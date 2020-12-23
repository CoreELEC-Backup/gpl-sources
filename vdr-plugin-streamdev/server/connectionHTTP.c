/*
 *  $Id: connectionHTTP.c,v 1.21 2010/08/03 10:46:41 schmirl Exp $
 */

#include <ctype.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <vdr/thread.h>
#include <vdr/recording.h>
 
#include "server/connectionHTTP.h"
#include "server/menuHTTP.h"
#include "server/server.h"
#include "server/setup.h"

cConnectionHTTP::cConnectionHTTP(void): 
		cServerConnection("HTTP"),
		m_Status(hsRequest),
		m_StreamType((eStreamType)StreamdevServerSetup.HTTPStreamType),
		m_Channel(NULL),
		m_RecPlayer(NULL),
		m_ReplayPos(0),
		m_ReplayFakeRange(false),
		m_MenuList(NULL)
{
	Dprintf("constructor hsRequest\n");
	m_Apid[0] = m_Apid[1] = 0;
	m_Dpid[0] = m_Dpid[1] = 0;
}

cConnectionHTTP::~cConnectionHTTP() 
{
	SetStreamer(NULL);
	delete m_RecPlayer;
	delete m_MenuList;
}

bool cConnectionHTTP::CanAuthenticate(void)
{
	return opt_auth != NULL;
}

bool cConnectionHTTP::Command(char *Cmd) 
{
	Dprintf("command %s\n", Cmd);
	switch (m_Status) {
	case hsRequest:
		// parse METHOD PATH[?QUERY] VERSION
		{
			char *p, *q, *v;
			p = strchr(Cmd, ' ');
			if (p) {
				*p = 0;
				v = strchr(++p, ' ');
				if (v) {
					*v = 0;
					SetHeader("REQUEST_METHOD", Cmd);
					q = strchr(p, '?');
					if (q) {
						*q = 0;
						SetHeader("QUERY_STRING", q + 1);
						while (q++) {
							char *n = strchr(q, '&');
							if (n)
								*n = 0;
							char *e = strchr(q, '=');
							if (e)
								*e++ = 0;
							else
								e = n ? n : v;
							m_Params.insert(tStrStr(q, e));
							q = n;
						}
					}
					else
						SetHeader("QUERY_STRING", "");
					SetHeader("PATH_INFO", p);
					m_Status = hsHeaders;
					return true;
				}
			}
		}
		return false;

	case hsHeaders:
		if (*Cmd == '\0') {
			m_Status = hsBody;
			return ProcessRequest();
		}
		else if (isspace(*Cmd)) {
			; //TODO: multi-line header
		}
		else {
			// convert header name to CGI conventions:
			// uppercase, '-' replaced with '_', prefix "HTTP_"
			char *p;
			for (p = Cmd; *p != 0 && *p != ':'; p++) {
				if (*p == '-')
					*p = '_';
				else
					*p = toupper(*p);
			}
			if (*p == ':') {
				*p = 0;
				p = skipspace(++p);
				// don't disclose Authorization header
				if (strcmp(Cmd, "AUTHORIZATION") == 0) {
					char *q;
					for (q = p; *q != 0 && *q != ' '; q++)
						*q = toupper(*q);
					if (p != q) {
						*q = 0;
						SetHeader("AUTH_TYPE", p);
						m_Authorization = (std::string) skipspace(++q);
					}
				}
				else
					SetHeader(Cmd, p, "HTTP_");
			}
		}
		return true;
	default:
		// skip additional blank lines
		if (*Cmd == '\0')
			return true;
		break;
	}
	return false; // ??? shouldn't happen
}

bool cConnectionHTTP::ProcessRequest(void) 
{
	// keys for Headers() hash
	const static std::string AUTH_TYPE("AUTH_TYPE");
	const static std::string REQUEST_METHOD("REQUEST_METHOD");
	const static std::string PATH_INFO("PATH_INFO");

	Dprintf("process\n");
	if (!StreamdevHosts.Acceptable(RemoteIpAddr())) {
		bool authOk = opt_auth && !m_Authorization.empty();
		if (authOk) {
			tStrStrMap::const_iterator it = Headers().find(AUTH_TYPE);

			if (it == Headers().end()) {
				// no authorization header present
				authOk = false;
			}
			else if (it->second.compare("BASIC") == 0) {
				// basic auth
				authOk &= m_Authorization.compare(opt_auth) == 0;
			}
			else {
				// unsupported auth type
				authOk = false;
			}
		}
		if (!authOk) {
			isyslog("streamdev-server: HTTP authorization required");
			return HttpResponse(401, true, NULL, "WWW-authenticate: basic Realm=\"Streamdev-Server\"");
		}
	}

	tStrStrMap::const_iterator it;
	it = m_Params.find("apid");
	if (it != m_Params.end())
		m_Apid[0] = atoi(it->second.c_str());
	it = m_Params.find("dpid");
	if (it != m_Params.end())
		m_Dpid[0] = atoi(it->second.c_str());

	tStrStrMap::const_iterator it_method = Headers().find(REQUEST_METHOD);
	tStrStrMap::const_iterator it_pathinfo = Headers().find(PATH_INFO);
	if (it_method == Headers().end() || it_pathinfo == Headers().end()) {
		// should never happen
		esyslog("streamdev-server connectionHTTP: Missing method or pathinfo");
	} else if (it_method->second.compare("GET") == 0 && ProcessURI(it_pathinfo->second)) {
		if (m_MenuList)
			return Respond("%s", true, m_MenuList->HttpHeader().c_str());
		else if (m_Channel != NULL) {
			if (cStreamdevLiveStreamer::ProvidesChannel(m_Channel, StreamdevServerSetup.HTTPPriority)) {
				cStreamdevLiveStreamer* liveStreamer = new cStreamdevLiveStreamer(this, m_Channel, StreamdevServerSetup.HTTPPriority, m_StreamType, m_Apid[0] ? m_Apid : NULL, m_Dpid[0] ? m_Dpid : NULL);
				if (liveStreamer->GetDevice()) {
					SetStreamer(liveStreamer);
					if (!SetDSCP())
						LOG_ERROR_STR("unable to set DSCP sockopt");
					if (m_StreamType == stEXT) {
						return Respond("HTTP/1.0 200 OK");
					} else if (m_StreamType == stES && (m_Apid[0] || m_Dpid[0] || ISRADIO(m_Channel))) {
						return HttpResponse(200, false, "audio/mpeg", "icy-name: %s", m_Channel->Name());
					} else if (ISRADIO(m_Channel)) {
						return HttpResponse(200, false, "audio/mpeg");
					} else {
						return HttpResponse(200, false, "video/mpeg");
					}
				}
				SetStreamer(NULL);
				delete liveStreamer;
			}
			return HttpResponse(503, true);
		}
		else if (m_RecPlayer != NULL) {
			Dprintf("GET recording\n");
			bool isPes = m_RecPlayer->getCurrentRecording()->IsPesRecording();
			// no remuxing for old PES recordings
			if (isPes && m_StreamType != stPES)
				return HttpResponse(503, true);

			int64_t from, to;
			bool hasRange = ParseRange(from, to);

			cStreamdevRecStreamer* recStreamer;
			if (from == 0 && hasRange && m_ReplayFakeRange) {
				recStreamer = new cStreamdevRecStreamer(this, m_RecPlayer, m_StreamType, (int64_t) 0L, m_Apid[0] ? m_Apid : NULL, m_Dpid[0] ? m_Dpid : NULL);
				from += m_ReplayPos;
				if (to >= 0)
					to += m_ReplayPos;
			}
			else
				recStreamer = new cStreamdevRecStreamer(this, m_RecPlayer, m_StreamType, m_ReplayPos, m_Apid[0] ? m_Apid : NULL, m_Dpid[0] ? m_Dpid : NULL);
			SetStreamer(recStreamer);

			if (m_StreamType == stEXT)
				return Respond("HTTP/1.0 200 OK");
			else if (m_StreamType == stES && (m_Apid[0] || m_Dpid[0]))
				return HttpResponse(200, false, "audio/mpeg");

			const char* contentType = (isPes || m_RecPlayer->getPatPmtData()->Vpid()) ? "video/mpeg" : "audio/mpeg";
			// range not supported when remuxing
			if (m_StreamType != stTS && !isPes)
				return HttpResponse(200, false, contentType);

			uint64_t total = recStreamer->GetLength();
			if (hasRange) {
				int64_t length = recStreamer->SetRange(from, to);
				Dprintf("range response: %lld-%lld/%lld, len %lld\n", (long long)from, (long long)to, (long long)total, (long long)length);
				if (length < 0L)
					return HttpResponse(416, true, contentType, "Accept-Ranges: bytes\r\nContent-Range: bytes */%llu", (unsigned long long) total);
				else
					return HttpResponse(206, false, contentType, "Accept-Ranges: bytes\r\nContent-Range: bytes %lld-%lld/%llu\r\nContent-Length: %lld", (long long) from, (long long) to, (unsigned long long) total, (long long) length);
			}
			else
				return HttpResponse(200, false, contentType, "Accept-Ranges: bytes");
		}
		else {
			return HttpResponse(404, true);
		}
	} else if (it_method->second.compare("HEAD") == 0 && ProcessURI(it_pathinfo->second)) {
		if (m_MenuList) {
			DeferClose();
			return Respond("%s", true, m_MenuList->HttpHeader().c_str());
		}
		else if (m_Channel != NULL) {
			if (cStreamdevLiveStreamer::ProvidesChannel(m_Channel, StreamdevServerSetup.HTTPPriority)) {
				if (m_StreamType == stEXT) {
					cStreamdevLiveStreamer *liveStreamer = new cStreamdevLiveStreamer(this, m_Channel, IDLEPRIORITY, m_StreamType, m_Apid[0] ? m_Apid : NULL, m_Dpid[0] ? m_Dpid : NULL);
					SetStreamer(liveStreamer);
					return Respond("HTTP/1.0 200 OK");
				} else if (m_StreamType == stES && (m_Apid[0] || m_Dpid[0] || ISRADIO(m_Channel))) {
					return HttpResponse(200, true, "audio/mpeg", "icy-name: %s", m_Channel->Name());
				} else if (ISRADIO(m_Channel)) {
					return HttpResponse(200, true, "audio/mpeg");
				} else {
					return HttpResponse(200, true, "video/mpeg");
				}
			}
			return HttpResponse(503, true);
		}
		else if (m_RecPlayer != NULL) {
			Dprintf("HEAD recording\n");
			bool isPes = m_RecPlayer->getCurrentRecording()->IsPesRecording();
			// no remuxing for old PES recordings
			if (isPes && m_StreamType != stPES)
				return HttpResponse(503, true);

			int64_t from, to;
			bool hasRange = ParseRange(from, to);
			
			cStreamdevRecStreamer* recStreamer;
			if (from == 0 && hasRange && m_ReplayFakeRange) {
				recStreamer = new cStreamdevRecStreamer(this, m_RecPlayer, m_StreamType, m_ReplayPos, m_Apid[0] ? m_Apid : NULL, m_Dpid[0] ? m_Dpid : NULL);
				from += m_ReplayPos;
				if (to >= 0)
					to += m_ReplayPos;
			}
			else
				recStreamer = new cStreamdevRecStreamer(this, m_RecPlayer, m_StreamType, m_ReplayPos, m_Apid[0] ? m_Apid : NULL, m_Dpid[0] ? m_Dpid : NULL);
			SetStreamer(recStreamer);

			if (m_StreamType == stEXT)
				return Respond("HTTP/1.0 200 OK");
			else if (m_StreamType == stES && (m_Apid[0] || m_Dpid[0]))
				return HttpResponse(200, true, "audio/mpeg");

			const char* contentType = (isPes || m_RecPlayer->getPatPmtData()->Vpid()) ? "video/mpeg" : "audio/mpeg";
			// range not supported when remuxing
			if (m_StreamType != stTS && !isPes)
				return HttpResponse(200, false, contentType);

			uint64_t total = recStreamer->GetLength();
			if (hasRange) {
				int64_t length = recStreamer->SetRange(from, to);
				if (length < 0L)
					return HttpResponse(416, true, contentType, "Accept-Ranges: bytes\r\nContent-Range: bytes */%llu", (unsigned long long) total);
				else
					return HttpResponse(206, true, contentType, "Accept-Ranges: bytes\r\nContent-Range: bytes %lld-%lld/%llu\r\nContent-Length: %lld", (long long) from, (long long) to, (unsigned long long) total, (long long) length);
			}
			else
				return HttpResponse(200, true, contentType, "Accept-Ranges: bytes");
		}
		else {
			return HttpResponse(404, true);
		}
	}

	return HttpResponse(400, true);
}

static const char *AAA[] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
static const char *MMM[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

bool cConnectionHTTP::HttpResponse(int Code, bool Last, const char* ContentType, const char* Headers, ...)
{
        va_list ap;
        va_start(ap, Headers);
#if APIVERSNUM >= 10728
        cString headers = cString::vsprintf(Headers, ap);
#else
        cString headers = cString::sprintf(Headers, ap);
#endif
        va_end(ap);

	bool rc;
	if (Last)
		DeferClose();
	switch (Code)
	{
		case 200: rc = Respond("HTTP/1.1 200 OK"); break;
		case 206: rc = Respond("HTTP/1.1 206 Partial Content"); break;
		case 400: rc = Respond("HTTP/1.1 400 Bad Request"); break;
		case 401: rc = Respond("HTTP/1.1 401 Authorization Required"); break;
		case 404: rc = Respond("HTTP/1.1 404 Not Found"); break;
		case 416: rc = Respond("HTTP/1.1 416 Requested range not satisfiable"); break;
		case 503: rc = Respond("HTTP/1.1 503 Service Unavailable"); break;
		default:  rc = Respond("HTTP/1.1 500 Internal Server Error");
	}
	if (rc && ContentType)
		rc = Respond("Content-Type: %s", true, ContentType);
	
	if (rc)
		rc = Respond("Connection: close")
			&& Respond("Pragma: no-cache")
			&& Respond("Cache-Control: no-cache")
			&& Respond("Server: VDR-%s / streamdev-server-%s", true, VDRVERSION, VERSION);

	time_t t = time(NULL);
	struct tm *gmt = gmtime(&t);
	if (rc && gmt) {
		char buf[] = "Date: AAA, DD MMM YYYY HH:MM:SS GMT";
		if (snprintf(buf, sizeof(buf), "Date: %s, %.2d %s %.4d %.2d:%.2d:%.2d GMT", AAA[gmt->tm_wday], gmt->tm_mday, MMM[gmt->tm_mon], gmt->tm_year + 1900, gmt->tm_hour, gmt->tm_min, gmt->tm_sec) == sizeof(buf) - 1)
			rc = Respond(buf);
	}

	if (rc && strlen(Headers) > 0)
		rc = Respond(headers);

	tStrStrMap::iterator it = m_Params.begin();
	while (rc && it != m_Params.end()) {
		static const char DLNA_POSTFIX[] = ".dlna.org";
		if (it->first.rfind(DLNA_POSTFIX) + sizeof(DLNA_POSTFIX) - 1 == it->first.length())
			rc = Respond("%s: %s", true, it->first.c_str(), it->second.c_str());
		++it;
	}
	return rc && Respond("");
}

bool cConnectionHTTP::ParseRange(int64_t &From, int64_t &To) const
{
	const static std::string RANGE("HTTP_RANGE");
	From = To = 0L;
	tStrStrMap::const_iterator it = Headers().find(RANGE);
	if (it != Headers().end()) {
		size_t b = it->second.find("bytes=");
		if (b != std::string::npos) {
			char* e = NULL;
			const char* r = it->second.c_str() + b + sizeof("bytes=") - 1;
			if (strchr(r, ',') != NULL)
				esyslog("streamdev-server cConnectionHTTP::GetRange: Multi-ranges not supported");
			From = strtol(r, &e, 10);
			if (r != e) {
				if (From < 0L) {
					To = -1L;
					return *e == 0 || *e == ',';
				}
				else if (*e == '-') {
					r = e + 1;
					if (*r == 0 || *e == ',') {
						To = -1L;
						return true;
					}
					To = strtol(r, &e, 10);
					return r != e && To >= From &&
							(*e == 0 || *e == ',');
				}
			}
		}
	}
	return false;
}

void cConnectionHTTP::Flushed(void) 
{
	if (m_Status != hsBody)
		return;

	if (m_MenuList) {
		if (m_MenuList->HasNext()) {
			if (!Respond("%s", true, m_MenuList->Next().c_str()))
				DeferClose();
		}
		else {
			DELETENULL(m_MenuList);
			m_Status = hsFinished;
			DeferClose();
		}
		return;
	}
	else if (Streamer()) {
		Dprintf("streamer start\n");
		Streamer()->Start(this);
		m_Status = hsFinished;
	}
	else {
		// should never be reached
		esyslog("streamdev-server cConnectionHTTP::Flushed(): no job to do");
		m_Status = hsFinished;
	}
}

cMenuList* cConnectionHTTP::MenuListFromString(const std::string& Path, const std::string& Filebase, const std::string& Fileext) const
{
	std::string groupTarget;
	cItemIterator *iterator = NULL;

	const static std::string GROUP("group");
	if (Filebase.compare("tree") == 0) {
		tStrStrMap::const_iterator it = m_Params.find(GROUP);
		iterator = new cListTree(it == m_Params.end() ? NULL : it->second.c_str());
		groupTarget = Filebase + Fileext;
	} else if (Filebase.compare("groups") == 0) {
		iterator = new cListGroups();
		groupTarget = (std::string) "group" + Fileext;
	} else if (Filebase.compare("group") == 0) {
		tStrStrMap::const_iterator it = m_Params.find(GROUP);
		iterator = new cListGroup(it == m_Params.end() ? NULL : it->second.c_str());
	} else if (Filebase.compare("channels") == 0) {
		iterator = new cListChannels();
	} else if (Filebase.compare("all") == 0 ||
			(Filebase.empty() && Fileext.empty())) {
		iterator = new cListAll();
	} else if (Filebase.compare("recordings") == 0) {
		iterator = new cRecordingsIterator(m_StreamType);
	}

	if (iterator) {
		// assemble base url: http://host/path/
		std::string base;
		const static std::string HOST("HTTP_HOST");
		tStrStrMap::const_iterator it = Headers().find(HOST);
		if (it != Headers().end())
			base = "http://" + it->second + "/";
		else
			base = (std::string) "http://" + LocalIp() + ":" +
				(const char*) itoa(StreamdevServerSetup.HTTPServerPort) + "/";
		base += Path;

		if (Filebase.empty() || Fileext.compare(".htm") == 0 || Fileext.compare(".html") == 0) {
			std::string self = Filebase + Fileext;
			std::string rss = Filebase + ".rss";
			tStrStrMap::const_iterator it = Headers().find("QUERY_STRING");
			if (it != Headers().end() && !it->second.empty()) {
				self += '?' + it->second;
				rss += '?' + it->second;
			}
			return new cHtmlMenuList(iterator, m_StreamType, self.c_str(), rss.c_str(), groupTarget.c_str());
		} else if (Fileext.compare(".m3u") == 0) {
			return new cM3uMenuList(iterator, base.c_str());
		} else if (Fileext.compare(".rss") == 0) {
			std::string html = Filebase + ".html";
			tStrStrMap::const_iterator it = Headers().find("QUERY_STRING");
			if (it != Headers().end() && !it->second.empty()) {
				html += '?' + it->second;
			}
			return new cRssMenuList(iterator, base.c_str(), html.c_str());
		} else {
			delete iterator;
		}
	}
	return NULL;
}

RecPlayer* cConnectionHTTP::RecPlayerFromString(const char *FileBase, const char *FileExt)
{
	RecPlayer *recPlayer = NULL;

	if (strcasecmp(FileExt, ".rec") != 0)
		return NULL;

	char *p = NULL;
	unsigned long l = strtoul(FileBase, &p, 0);
	if (p != FileBase && l > 0L) {
		if (*p == ':') {
			// get recording by dev:inode
			ino_t inode = (ino_t) strtoull(p + 1, &p, 0);
			if (*p == 0 && inode > 0) {
				struct stat st;
#if APIVERSNUM >= 20300
				LOCK_RECORDINGS_READ;
				for (const cRecording *rec = Recordings->First(); rec; rec = Recordings->Next(rec)) {
#else
				cThreadLock RecordingsLock(&Recordings);
				for (cRecording *rec = Recordings.First(); rec; rec = Recordings.Next(rec)) {
#endif
					if (stat(rec->FileName(), &st) == 0 && st.st_dev == (dev_t) l && st.st_ino == inode)
						recPlayer = new RecPlayer(rec->FileName());
				}
			}
		}
		else if (*p == 0) {
			// get recording by index
#if APIVERSNUM >= 20300
			LOCK_RECORDINGS_READ;
			const cRecording *rec = Recordings->Get((int) l - 1);
#else
			cThreadLock RecordingsLock(&Recordings);
			cRecording *rec = Recordings.Get((int) l - 1);
#endif
			if (rec)
				recPlayer = new RecPlayer(rec->FileName());
		}

		if (recPlayer) {
			const char *pos = NULL;
			tStrStrMap::const_iterator it = m_Params.begin();
			while (it != m_Params.end()) {
				if (it->first == "pos") {
					pos = it->second.c_str();
					break;
				}
				++it;
			}
			if (pos) {
				// With prefix "full_" we try to fool players
				// by replying with a content range starting
				// at the requested position instead of 0.
				// This is a heavy violation of standards.
				// Use at your own risk!
				if (strncasecmp(pos, "full_", 5) == 0) {
					m_ReplayFakeRange = true;
					pos += 5;
				}
				if (strncasecmp(pos, "resume", 6) == 0) {
					int id = pos[6] == '.' ? atoi(pos + 7) : 0;
					m_ReplayPos = recPlayer->positionFromResume(id);
				}
				else if (strncasecmp(pos, "mark.", 5) == 0) {
					int index = atoi(pos + 5);
					m_ReplayPos = recPlayer->positionFromMark(index);
				}
				else if (strncasecmp(pos, "time.", 5) == 0) {
					int seconds = atoi(pos + 5);
					m_ReplayPos = recPlayer->positionFromTime(seconds);
				}
				else if (strncasecmp(pos, "frame.", 6) == 0) {
					int frame = atoi(pos + 6);
					m_ReplayPos = recPlayer->positionFromFrameNumber(frame);
				}
				else {
					m_ReplayPos = atol(pos);
					if (m_ReplayPos > 0L && m_ReplayPos < 100L)
						m_ReplayPos = recPlayer->positionFromPercent((int) m_ReplayPos);
				}
			}
		}
	}
	return recPlayer;
}

bool cConnectionHTTP::ProcessURI(const std::string& PathInfo) 
{
	std::string filespec, fileext;
	size_t file_pos = PathInfo.rfind('/');

	if (file_pos != std::string::npos) {
		size_t ext_pos = PathInfo.rfind('.');
		if (ext_pos != std::string::npos) {
			// file extension including leading .
			fileext = PathInfo.substr(ext_pos);
			const char *ext = fileext.c_str();
			// ignore dummy file extensions
			if (strcasecmp(ext, ".ts") == 0 ||
					strcasecmp(ext, ".vdr") == 0 ||
					strcasecmp(ext, ".vob") == 0) {
				size_t ext_end = ext_pos;
				if (ext_pos > 0)
					ext_pos = PathInfo.rfind('.', ext_pos - 1);
				if (ext_pos == std::string::npos)
					ext_pos = ext_end;
				fileext = PathInfo.substr(ext_pos, ext_end - ext_pos);
			}
		}
		// file basename with leading / stripped off
		filespec = PathInfo.substr(file_pos + 1, ext_pos - file_pos - 1);
	}
	if (fileext.length() > 5) {
		//probably not an extension
		filespec += fileext;
		fileext.clear();
	}

	// Streamtype with leading / stripped off
	std::string type = PathInfo.substr(1, PathInfo.find_first_of("/;", 1) - 1);
	const char* pType = type.c_str();
	if (strcasecmp(pType, "PES") == 0) {
		m_StreamType = stPES;
#ifdef STREAMDEV_PS
	} else if (strcasecmp(pType, "PS") == 0) {
		m_StreamType = stPS;
#endif
	} else if (strcasecmp(pType, "TS") == 0) {
		m_StreamType = stTS;
	} else if (strcasecmp(pType, "ES") == 0) {
		m_StreamType = stES;
	} else if (strcasecmp(pType, "EXT") == 0) {
		m_StreamType = stEXT;
	}

	Dprintf("before channelfromstring: type(%s) filespec(%s) fileext(%s)\n", type.c_str(), filespec.c_str(), fileext.c_str());

	if ((m_MenuList = MenuListFromString(PathInfo.substr(1, file_pos), filespec.c_str(), fileext.c_str())) != NULL) {
		Dprintf("Channel list requested\n");
		return true;
	} else if ((m_RecPlayer = RecPlayerFromString(filespec.c_str(), fileext.c_str())) != NULL) {
		Dprintf("Recording %s found\n", m_RecPlayer->getCurrentRecording()->Name());
		return true;
	} else if ((m_Channel = ChannelFromString(filespec.c_str(), &m_Apid[0], &m_Dpid[0])) != NULL) {
		Dprintf("Channel found. Apid/Dpid is %d/%d\n", m_Apid[0], m_Dpid[0]);
		return true;
	} else
		return false;
}

cString cConnectionHTTP::ToText(char Delimiter) const
{
	cString str = cServerConnection::ToText(Delimiter);
	return Streamer() ? cString::sprintf("%s%c%s", *str, Delimiter, *Streamer()->ToText()) : str;
}
