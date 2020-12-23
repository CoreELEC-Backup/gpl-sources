/*
 *  $Id: componentIGMP.c,v 1.2 2009/07/03 21:44:19 schmirl Exp $
 */
#include <netinet/ip.h>
#include <netinet/igmp.h>
 
#include "server/componentIGMP.h"
#include "server/connectionIGMP.h"
#include "server/server.h"
#include "server/setup.h"

#ifndef IGMP_ALL_HOSTS
#define IGMP_ALL_HOSTS htonl(0xE0000001L)
#endif
#ifndef IGMP_ALL_ROUTER
#define IGMP_ALL_ROUTER htonl(0xE0000002L)
#endif

// IGMP parameters according to RFC2236. All time values in seconds.
#define IGMP_ROBUSTNESS 2
#define IGMP_QUERY_INTERVAL 125
#define IGMP_QUERY_RESPONSE_INTERVAL 10
#define IGMP_GROUP_MEMBERSHIP_INTERVAL (2 * IGMP_QUERY_INTERVAL + IGMP_QUERY_RESPONSE_INTERVAL)
#define IGMP_OTHER_QUERIER_PRESENT_INTERVAL (2 * IGMP_QUERY_INTERVAL + IGMP_QUERY_RESPONSE_INTERVAL / 2)
#define IGMP_STARTUP_QUERY_INTERVAL (IGMP_QUERY_INTERVAL / 4)
#define IGMP_STARTUP_QUERY_COUNT IGMP_ROBUSTNESS
// This value is 1/10 sec. RFC default is 10. Reduced to minimum to free unused channels ASAP
#define IGMP_LAST_MEMBER_QUERY_INTERVAL_TS 1
#define IGMP_LAST_MEMBER_QUERY_COUNT IGMP_ROBUSTNESS

// operations on struct timeval
#define TV_CMP(a, cmp, b) (a.tv_sec == b.tv_sec ? a.tv_usec cmp b.tv_usec : a.tv_sec cmp b.tv_sec)
#define TV_SET(tv) (tv.tv_sec || tv.tv_usec)
#define TV_CLR(tv) memset(&tv, 0, sizeof(tv))
#define TV_CPY(dst, src) memcpy(&dst, &src, sizeof(dst))
#define TV_ADD(dst, ts) dst.tv_sec += ts / 10; dst.tv_usec += (ts % 10) * 100000; if (dst.tv_usec >= 1000000) { dst.tv_usec -= 1000000; dst.tv_sec++; }

class cMulticastGroup: public cListObject
{
public:
	in_addr_t group;
	in_addr_t reporter;
	struct timeval timeout;
	struct timeval v1timer;
	struct timeval retransmit;

	cMulticastGroup(in_addr_t Group);
};

cMulticastGroup::cMulticastGroup(in_addr_t Group) :
		group(Group),
		reporter(0)
{
	TV_CLR(timeout);
	TV_CLR(v1timer);
	TV_CLR(retransmit);
}

void logIGMP(uint8_t type, struct in_addr Src, struct in_addr Dst, struct in_addr Grp)
{
	const char* msg;
	switch (type) {
		case IGMP_MEMBERSHIP_QUERY:	msg = "membership query"; break;
		case IGMP_V1_MEMBERSHIP_REPORT:	msg = "V1 membership report"; break;
		case IGMP_V2_MEMBERSHIP_REPORT:	msg = "V2 membership report"; break;
		case IGMP_V2_LEAVE_GROUP:	msg = "leave group"; break;
		default:			msg = "unknown"; break;
	}
	char* s = strdup(inet_ntoa(Src));
	char* d = strdup(inet_ntoa(Dst));
	dsyslog("streamdev-server IGMP: Received %s from %s (dst %s) for %s", msg, s, d, inet_ntoa(Grp));
	free(s);
	free(d);
}

/* Taken from http://tools.ietf.org/html/rfc1071 */
uint16_t inetChecksum(uint16_t *addr, int count)
{
	uint32_t sum = 0;
	while (count > 1) {
		sum += *addr++;
		count -= 2;
	}

        if( count > 0 )
		sum += * (uint8_t *) addr;

	while (sum>>16)
		sum = (sum & 0xffff) + (sum >> 16);

	return ~sum;
}

cComponentIGMP::cComponentIGMP(void):
		cServerComponent("IGMP", "0.0.0.0", 0, SOCK_RAW, IPPROTO_IGMP),
		cThread("IGMP timeout handler"),
		m_BindIp(inet_addr(StreamdevServerSetup.IGMPBindIP)),
		m_MaxChannelNumber(0),
		m_StartupQueryCount(IGMP_STARTUP_QUERY_COUNT),
		m_Querier(true)
{
}

cComponentIGMP::~cComponentIGMP(void)
{
}

#if APIVERSNUM >= 20300
cMulticastGroup* cComponentIGMP::FindGroup(in_addr_t Group)
#else
cMulticastGroup* cComponentIGMP::FindGroup(in_addr_t Group) const
#endif
{
	cMulticastGroup *group = m_Groups.First();
	while (group && group->group != Group)
		group = m_Groups.Next(group);
	return group;
}

bool cComponentIGMP::Initialize(void)
{
	if (cServerComponent::Initialize() && IGMPMembership(IGMP_ALL_ROUTER))
	{
#if APIVERSNUM >= 20300
		LOCK_CHANNELS_READ;
		for (const cChannel *channel = Channels->First(); channel; channel = Channels->Next(channel))
#else
		for (cChannel *channel = Channels.First(); channel; channel = Channels.Next(channel))
#endif
		{
			if (channel->GroupSep())
				continue;
			int num = channel->Number();
			if (!IGMPMembership(htonl(MULTICAST_PRIV_MIN + num)))
				break;
			m_MaxChannelNumber = num;
		}
		if (m_MaxChannelNumber == 0)
		{
			IGMPMembership(IGMP_ALL_ROUTER, false);
			esyslog("streamdev-server IGMP: no multicast group joined");
		}
		else
		{
			Start();
		}
	}
	return m_MaxChannelNumber > 0;
}

void cComponentIGMP::Destruct(void)
{
	if (m_MaxChannelNumber > 0)
	{
		Cancel(-1);
		m_CondWait.Signal();
		Cancel(2);
#if APIVERSNUM >= 20300
		LOCK_CHANNELS_READ;
		for (const cChannel *channel = Channels->First(); channel; channel = Channels->Next(channel))
#else
		for (cChannel *channel = Channels.First(); channel; channel = Channels.Next(channel))
#endif
		{
			if (channel->GroupSep())
				continue;
			int num = channel->Number();
			if (num > m_MaxChannelNumber)
				break;
			IGMPMembership(htonl(MULTICAST_PRIV_MIN + num), false);
		}
		IGMPMembership(IGMP_ALL_ROUTER, false);
	}
	m_MaxChannelNumber = 0;
	cServerComponent::Destruct();
}

cServerConnection *cComponentIGMP::NewClient(void)
{
	return new cConnectionIGMP("IGMP", StreamdevServerSetup.IGMPClientPort, (eStreamType) StreamdevServerSetup.IGMPStreamType);
}

cServerConnection* cComponentIGMP::Accept(void)
{
	ssize_t recv_len;
	int ip_hdrlen, ip_datalen;
	struct ip *ip;
	struct igmp *igmp;

	while ((recv_len = ::recvfrom(Socket(), m_ReadBuffer, sizeof(m_ReadBuffer), 0, NULL, NULL)) < 0 && errno == EINTR)
		errno = 0;

	if (recv_len < 0) {
		esyslog("streamdev-server IGMP: read failed: %m");
		return NULL;
	}
	else if (recv_len < (ssize_t) sizeof(struct ip)) {
		esyslog("streamdev-server IGMP: IP packet too short");
		return NULL;
	}

	ip = (struct ip*) m_ReadBuffer;

	// filter out my own packets
	if (ip->ip_src.s_addr == m_BindIp)
		return NULL;

	ip_hdrlen = ip->ip_hl << 2;
#ifdef __FreeBSD__
	ip_datalen = ip->ip_len;
#else
	ip_datalen = ntohs(ip->ip_len) - ip_hdrlen;
#endif
	if (ip->ip_p != IPPROTO_IGMP) {
		esyslog("streamdev-server IGMP: Unexpected protocol %hhu", ip->ip_p);
		return NULL;
	}
	if (recv_len < ip_hdrlen + IGMP_MINLEN) {
		esyslog("streamdev-server IGMP: packet too short");
		return NULL;
	}
	igmp = (struct igmp*) (m_ReadBuffer + ip_hdrlen);
	uint16_t chksum = igmp->igmp_cksum;
	igmp->igmp_cksum = 0;
	if (chksum != inetChecksum((uint16_t *)igmp, ip_datalen))
	{
		esyslog("INVALID CHECKSUM %d %d %d %lu 0x%x 0x%x", (int) ntohs(ip->ip_len), ip_hdrlen, ip_datalen, (unsigned long int) recv_len, chksum, inetChecksum((uint16_t *)igmp, ip_datalen));
		return NULL;
	}
	logIGMP(igmp->igmp_type, ip->ip_src, ip->ip_dst, igmp->igmp_group);
	return ProcessMessage(igmp, igmp->igmp_group.s_addr, ip->ip_src.s_addr);
}

cServerConnection* cComponentIGMP::ProcessMessage(struct igmp *Igmp, in_addr_t Group, in_addr_t Sender)
{
	cServerConnection* conn = NULL;
	cMulticastGroup* group;
	LOCK_THREAD;
	switch (Igmp->igmp_type) {
		case IGMP_MEMBERSHIP_QUERY:
			if (ntohl(Sender) < ntohl(m_BindIp))
				IGMPStartOtherQuerierPresentTimer();
			break;
		case IGMP_V1_MEMBERSHIP_REPORT:
		case IGMP_V2_MEMBERSHIP_REPORT:
			group = FindGroup(Group);
			if (!group) {
				group = new cMulticastGroup(Group);
				m_Groups.Add(group);
			}
			conn = IGMPStartMulticast(group);
			IGMPStartTimer(group, Sender);
			if (Igmp->igmp_type == IGMP_V1_MEMBERSHIP_REPORT)
				IGMPStartV1HostTimer(group);
			break;
		case IGMP_V2_LEAVE_GROUP:
			group = FindGroup(Group);
			if (group && !TV_SET(group->v1timer)) {
				if (group->reporter == Sender) {
					IGMPStartTimerAfterLeave(group, m_Querier ? IGMP_LAST_MEMBER_QUERY_INTERVAL_TS : Igmp->igmp_code);
					if (m_Querier)
						IGMPSendGroupQuery(group);
					IGMPStartRetransmitTimer(group);
				}
				m_CondWait.Signal();
			}
			break;
		default:
			break;
	}
	return conn;
}

void cComponentIGMP::Action()
{
	while (Running()) {
		struct timeval now;
		struct timeval next;

		gettimeofday(&now, NULL);
		TV_CPY(next, now);
		next.tv_sec += IGMP_QUERY_INTERVAL;

		cMulticastGroup *del = NULL;
		{
			LOCK_THREAD;
			if (TV_CMP(m_GeneralQueryTimer, <, now)) {
				dsyslog("General Query");
				IGMPSendGeneralQuery();
				IGMPStartGeneralQueryTimer();
			}
			if (TV_CMP(next, >, m_GeneralQueryTimer))
				TV_CPY(next, m_GeneralQueryTimer);

			for (cMulticastGroup *group = m_Groups.First(); group; group = m_Groups.Next(group)) {
				if (TV_CMP(group->timeout, <, now)) {
					IGMPStopMulticast(group);
					IGMPClearRetransmitTimer(group);
					if (del)
						m_Groups.Del(del);
					del = group;
				}
				else if (m_Querier && TV_SET(group->retransmit) && TV_CMP(group->retransmit, <, now)) {
					IGMPSendGroupQuery(group);
					IGMPStartRetransmitTimer(group);
					if (TV_CMP(next, >, group->retransmit))
						TV_CPY(next, group->retransmit);
				}
				else if (TV_SET(group->v1timer) && TV_CMP(group->v1timer, <, now)) {
					TV_CLR(group->v1timer);
				}
				else {
					if (TV_CMP(next, >, group->timeout))
						TV_CPY(next, group->timeout);
					if (TV_SET(group->retransmit) && TV_CMP(next, >, group->retransmit))
						TV_CPY(next, group->retransmit);
					if (TV_SET(group->v1timer) && TV_CMP(next, >, group->v1timer))
						TV_CPY(next, group->v1timer);
				}
			}
			if (del)
				m_Groups.Del(del);
		}
		
		int sleep = (next.tv_sec - now.tv_sec) * 1000;
		sleep += (next.tv_usec - now.tv_usec) / 1000;
		if (next.tv_usec < now.tv_usec)
			sleep += 1000;
		dsyslog("Sleeping %d ms", sleep);
		m_CondWait.Wait(sleep);
	}
}

bool cComponentIGMP::IGMPMembership(in_addr_t Group, bool Add)
{
	struct ip_mreqn mreq;
	mreq.imr_multiaddr.s_addr = Group;
	mreq.imr_address.s_addr = INADDR_ANY;
	mreq.imr_ifindex = 0;
	if (setsockopt(Socket(), IPPROTO_IP, Add ? IP_ADD_MEMBERSHIP : IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
	{
		esyslog("streamdev-server IGMP: unable to %s %s: %m", Add ? "join" : "leave", inet_ntoa(mreq.imr_multiaddr));
		if (errno == ENOBUFS)
			esyslog("consider increasing sys.net.ipv4.igmp_max_memberships");
		return false;
	}
	return true;
}

void cComponentIGMP::IGMPSendQuery(in_addr_t Group, int Timeout)
{
	struct sockaddr_in dst;
	struct igmp query;

	dst.sin_family = AF_INET;
	dst.sin_port = IPPROTO_IGMP;
	dst.sin_addr.s_addr = Group;
	query.igmp_type = IGMP_MEMBERSHIP_QUERY;
	query.igmp_code = Timeout * 10;
	query.igmp_cksum = 0;
	query.igmp_group.s_addr = (Group == IGMP_ALL_HOSTS) ? 0 : Group;
	query.igmp_cksum = inetChecksum((uint16_t *) &query, sizeof(query));

	for (int i = 0; i < 5 && ::sendto(Socket(), &query, sizeof(query), 0, (sockaddr*)&dst, sizeof(dst)) == -1; i++) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			esyslog("streamdev-server IGMP: unable to query group %s: %m", inet_ntoa(dst.sin_addr));
			break;
		}
		cCondWait::SleepMs(10);
	}
}

// Querier state actions
void cComponentIGMP::IGMPStartGeneralQueryTimer()
{
	m_Querier = true;
	if (m_StartupQueryCount) {
		gettimeofday(&m_GeneralQueryTimer, NULL);
		m_GeneralQueryTimer.tv_sec += IGMP_STARTUP_QUERY_INTERVAL;
		m_StartupQueryCount--;
	}
	else {
		gettimeofday(&m_GeneralQueryTimer, NULL);
		m_GeneralQueryTimer.tv_sec += IGMP_QUERY_INTERVAL;
	}
}

void cComponentIGMP::IGMPStartOtherQuerierPresentTimer()
{
	m_Querier = false;
	m_StartupQueryCount = 0;
	gettimeofday(&m_GeneralQueryTimer, NULL);
	m_GeneralQueryTimer.tv_sec += IGMP_OTHER_QUERIER_PRESENT_INTERVAL;
}

void cComponentIGMP::IGMPSendGeneralQuery()
{
	IGMPSendQuery(IGMP_ALL_HOSTS, IGMP_QUERY_RESPONSE_INTERVAL);
}

// Group state actions
void cComponentIGMP::IGMPStartTimer(cMulticastGroup* Group, in_addr_t Member)
{
	gettimeofday(&Group->timeout, NULL);
	Group->timeout.tv_sec += IGMP_GROUP_MEMBERSHIP_INTERVAL;
	TV_CLR(Group->retransmit);
	Group->reporter = Member;

}

void cComponentIGMP::IGMPStartV1HostTimer(cMulticastGroup* Group)
{
	gettimeofday(&Group->v1timer, NULL);
	Group->v1timer.tv_sec += IGMP_GROUP_MEMBERSHIP_INTERVAL;
}

void cComponentIGMP::IGMPStartTimerAfterLeave(cMulticastGroup* Group, unsigned int MaxResponseTimeTs)
{
	//Group->Update(time(NULL) + MaxResponseTime * IGMP_LAST_MEMBER_QUERY_COUNT / 10);
	MaxResponseTimeTs *= IGMP_LAST_MEMBER_QUERY_COUNT;
	gettimeofday(&Group->timeout, NULL);
	TV_ADD(Group->timeout, MaxResponseTimeTs);
	TV_CLR(Group->retransmit);
	Group->reporter = 0;
}

void cComponentIGMP::IGMPStartRetransmitTimer(cMulticastGroup* Group)
{
	gettimeofday(&Group->retransmit, NULL);
	TV_ADD(Group->retransmit, IGMP_LAST_MEMBER_QUERY_INTERVAL_TS);
}

void cComponentIGMP::IGMPClearRetransmitTimer(cMulticastGroup* Group)
{
	TV_CLR(Group->retransmit);
}

void cComponentIGMP::IGMPSendGroupQuery(cMulticastGroup* Group)
{
	IGMPSendQuery(Group->group, IGMP_LAST_MEMBER_QUERY_INTERVAL_TS);
}

cServerConnection* cComponentIGMP::IGMPStartMulticast(cMulticastGroup* Group)
{
	cServerConnection *conn = NULL;
	in_addr_t g = ntohl(Group->group);
	if (g > MULTICAST_PRIV_MIN && g <= MULTICAST_PRIV_MAX) {
		cThreadLock lock;
#if APIVERSNUM >= 20300
		LOCK_CHANNELS_READ;
		const cChannel *channel = Channels->GetByNumber(g - MULTICAST_PRIV_MIN);
#else
		cChannel *channel = Channels.GetByNumber(g - MULTICAST_PRIV_MIN);
#endif
		const cList<cServerConnection>& clients = cStreamdevServer::Clients(lock);
#if APIVERSNUM >= 20300
		const cServerConnection *s = clients.First();
#else
		cServerConnection *s = clients.First();
#endif
		while (s) {
			if (s->RemoteIpAddr() == Group->group)
				break;
			s = clients.Next(s);
		}
		if (!s) {
			conn = NewClient();
			if (!((cConnectionIGMP *)conn)->SetChannel(channel, Group->group)) {
				DELETENULL(conn);
			}
		}
	}
	return conn;
}

void cComponentIGMP::IGMPStopMulticast(cMulticastGroup* Group)
{
	cThreadLock lock;
#if APIVERSNUM >= 20300
	cList<cServerConnection>& clients = cStreamdevServer::Clients(lock);
#else
	const cList<cServerConnection>& clients = cStreamdevServer::Clients(lock);
#endif
	for (cServerConnection *s = clients.First(); s; s = clients.Next(s)) {
		if (s->RemoteIpAddr() == Group->group)
			s->Close();
	}
}
