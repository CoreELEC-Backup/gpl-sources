/*
 *      vdr-plugin-robotv - roboTV server plugin for VDR
 *
 *      Copyright (C) 2016 Alexander Pipelka
 *
 *      https://github.com/pipelka/vdr-plugin-robotv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include <netdb.h>
#include <poll.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <vdr/plugin.h>
#include <vdr/shutdown.h>
#include <net/sdp.h>

#include "robotvserver.h"
#include "robotvclient.h"
#include "recordings/recordingscache.h"
#include "net/os-config.h"
#include "tools/hash.h"

unsigned int RoboTVServer::m_idCnt = 0;

class cAllowedHosts : public cSVDRPhosts {
public:
    cAllowedHosts(const cString& AllowedHostsFile) {
        char allHosts[15];
        strcpy(allHosts, "0.0.0.0/0");

        if(!Load(AllowedHostsFile, true, true)) {
            esyslog("Invalid or missing %s. Disabling access restrictions !!!.", *AllowedHostsFile);
            esyslog("Please create the file as soon as possible.");
            cSVDRPhost* localhost = new cSVDRPhost;

            if(localhost->Parse(allHosts)) {
                Add(localhost);
            }
            else {
                delete localhost;
            }
        }
    }
};

RoboTVServer::RoboTVServer(int listenPort) : cThread("roboTV VDR Server"), m_config(RoboTVServerConfig::instance()) {
    m_ipv4Fallback = false;
    m_serverPort  = listenPort;

    if(!m_config.configDirectory.empty()) {
        m_allowedHostsFile = cString::sprintf("%s/" ALLOWED_HOSTS_FILE, m_config.configDirectory.c_str());
    }
    else {
        esyslog("RoboTVServer: missing ConfigDirectory!");
        m_allowedHostsFile = cString::sprintf("/video/" ALLOWED_HOSTS_FILE);
    }

    m_serverFd = socket(AF_INET6, SOCK_STREAM, 0);

    if(m_serverFd == -1) {
        // trying to fallback to IPv4
        m_serverFd = socket(AF_INET, SOCK_STREAM, 0);

        if(m_serverFd == -1) {
            return;
        }
        else {
            m_ipv4Fallback = true;
        }
    }

    fcntl(m_serverFd, F_SETFD, fcntl(m_serverFd, F_GETFD) | FD_CLOEXEC);

    int one = 1;
    setsockopt(m_serverFd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

    // setting the server socket option to listen on IPv4 and IPv6 simultaneously
    int no = 0;

    if(!m_ipv4Fallback && setsockopt(m_serverFd, IPPROTO_IPV6, IPV6_V6ONLY, (void*)&no, sizeof(no)) < 0) {
        esyslog("RoboTVServer: setsockopt failed (errno=%d: %s)", errno, strerror(errno));
    }

    struct sockaddr_storage s;

    memset(&s, 0, sizeof(s));

    if(!m_ipv4Fallback) {
        ((struct sockaddr_in6*)&s)->sin6_family = AF_INET6;
        ((struct sockaddr_in6*)&s)->sin6_port = htons(m_serverPort);
    }
    else {
        ((struct sockaddr_in*)&s)->sin_family = AF_INET;
        ((struct sockaddr_in*)&s)->sin_port = htons(m_serverPort);
    }

    int x = bind(m_serverFd, (struct sockaddr*)&s, sizeof(s));

    if(x < 0) {
        close(m_serverFd);
        isyslog("Unable to start roboTV Server, port already in use ?");
        m_serverFd = -1;
        return;
    }

    Start();

    return;
}

RoboTVServer::~RoboTVServer() {
    Cancel(10);

    for(ClientList::iterator i = m_clients.begin(); i != m_clients.end(); i++) {
        delete(*i);
    }

    isyslog("roboTV Server stopped");
}

void RoboTVServer::clientConnected(int fd) {
    struct sockaddr_storage sin;
    socklen_t len = sizeof(sin);
    in_addr_t* ipv4_addr = NULL;

    if(getpeername(fd, (struct sockaddr*)&sin, &len)) {
        esyslog("getpeername() failed, dropping new incoming connection %d", m_idCnt);
        close(fd);
        return;
    }

    if(!m_ipv4Fallback) {
        if(IN6_IS_ADDR_V4MAPPED(&((struct sockaddr_in6*)&sin)->sin6_addr) ||
                IN6_IS_ADDR_V4COMPAT(&((struct sockaddr_in6*)&sin)->sin6_addr)) {
            ipv4_addr = &((struct sockaddr_in6*)&sin)->sin6_addr.s6_addr32[3];
        }
    }
    else {
        ipv4_addr = &((struct sockaddr_in*)&sin)->sin_addr.s_addr;
    }

    // Acceptable() method only supports in_addr_t argument so we're currently checking IPv4 hosts only
    if(ipv4_addr) {
        cAllowedHosts AllowedHosts(m_allowedHostsFile);

        if(!AllowedHosts.Acceptable(*ipv4_addr)) {
            esyslog("Address not allowed to connect (%s)", *m_allowedHostsFile);
            close(fd);
            return;
        }
    }

    if(fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == -1) {
        esyslog("Error setting control socket to nonblocking mode");
        close(fd);
        return;
    }

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val));

#ifndef __FreeBSD__
    val = 1;
    setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, &val, sizeof(val));

    val = 1;
    setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, &val, sizeof(val));

    val = 3;
    setsockopt(fd, SOL_TCP, TCP_KEEPCNT, &val, sizeof(val));
#endif

    val = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));

    if(!m_ipv4Fallback) {
        isyslog("Client %s:%i with ID %d connected.", robotv_inet_ntoa(((struct sockaddr_in6*)&sin)->sin6_addr), ((struct sockaddr_in6*)&sin)->sin6_port, m_idCnt);
    }
    else {
        isyslog("Client %s:%i with ID %d connected.", inet_ntoa(((struct sockaddr_in*)&sin)->sin_addr), ((struct sockaddr_in*)&sin)->sin_port, m_idCnt);
    }

    RoboTvClient* connection = new RoboTvClient(fd, m_idCnt);
    m_clients.push_back(connection);
    m_idCnt++;
}

void RoboTVServer::Action(void) {
    fd_set fds;
    struct timeval tv;

    // artwork
    Artwork artwork;
    cTimeMs cleanupTimer;

    isyslog("creating SDP client");
    roboTV::Sdp& sdp = roboTV::Sdp::createInstance();

    isyslog("removing outdated artwork");
    artwork.cleanup();

    RecordingsCache& cache = RecordingsCache::instance();
    {
        LOCK_RECORDINGS_READ;
        dsyslog("populating recordings cache");
        cache.update(Recordings);
    }

    // listen for connections
    listen(m_serverFd, 10);

    isyslog("roboTV Server started");

    while(Running()) {
        // poll sdp
        sdp.poll();

        // poll network socket
        FD_ZERO(&fds);
        FD_SET(m_serverFd, &fds);

        tv.tv_sec = 0;
        tv.tv_usec = 250 * 1000;

        int r = select(m_serverFd + 1, &fds, NULL, NULL, &tv);

        if(r == -1) {
            esyslog("failed during select");
            continue;
        }

        if(r == 0) {
            // remove disconnected clients
            for(ClientList::iterator i = m_clients.begin(); i != m_clients.end();) {

                if(!(*i)->Active()) {
                    isyslog("Client with ID %u seems to be disconnected, removing from client list", (*i)->getId());
                    delete(*i);
                    i = m_clients.erase(i);
                }
                else {
                    i++;
                }
            }

            // cleanup (every hour)
            if(cleanupTimer.Elapsed() >= 60 * 60 * 1000) {
                isyslog("removing outdated artwork");
                artwork.triggerCleanup();
                // start gc
                isyslog("Starting garbage collection in recordings cache");
                cache.triggerCleanup();

                cleanupTimer.Set(0);
            }

            // reset inactivity timeout as long as there are clients connected
            if(m_clients.size() > 0) {
                ShutdownHandler.SetUserInactiveTimeout();
            }

            // no connect request -> continue waiting
            continue;
        }

        int fd = accept(m_serverFd, 0, 0);

        if(fd >= 0) {
            clientConnected(fd);
        }
        else {
            esyslog("accept failed");
        }
    }

    return;
}
