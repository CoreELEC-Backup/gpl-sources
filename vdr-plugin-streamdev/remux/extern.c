#include "remux/extern.h"
#include "server/server.h"
#include "server/connection.h"
#include "server/streamer.h"
#include <vdr/channels.h>
#include <vdr/remux.h>
#include <vdr/tools.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

namespace Streamdev {

#define MAXENV 63

class cTSExt: public cThread {
private:
	cRingBufferLinear *m_ResultBuffer;
	bool               m_Active;
	int                m_Process;
	int                m_Inpipe, m_Outpipe;

protected:
	virtual void Action(void);

public:
	cTSExt(cRingBufferLinear *ResultBuffer, const cServerConnection *Connection, const cChannel *Channel, const cPatPmtParser *PatPmt, const int *Apids, const int *Dpids);
	virtual ~cTSExt();

	void Put(const uchar *Data, int Count);
};

} // namespace Streamdev
using namespace Streamdev;

cTSExt::cTSExt(cRingBufferLinear *ResultBuffer, const cServerConnection *Connection, const cChannel *Channel, const cPatPmtParser *PatPmt, const int *Apids, const int *Dpids):
		m_ResultBuffer(ResultBuffer),
		m_Active(false),
		m_Process(-1),
		m_Inpipe(0),
		m_Outpipe(0)
{
	int inpipe[2];
	int outpipe[2];

	if (pipe(inpipe) == -1) {
		LOG_ERROR_STR("pipe failed");
		return;
	}

	if (pipe(outpipe) == -1) {
		LOG_ERROR_STR("pipe failed");
		close(inpipe[0]);
		close(inpipe[1]);
		return;
	}

	if ((m_Process = fork()) == -1) {
		LOG_ERROR_STR("fork failed");
		close(inpipe[0]);
		close(inpipe[1]);
		close(outpipe[0]);
		close(outpipe[1]);
		return;
	}

	if (m_Process == 0) {
		// child process
		char *env[MAXENV + 1];
		int i = 0;

#define ADDENV(x...) if (asprintf(&env[i++], x) < 0) i--

		// add channel ID, name and pids to environment
		if (Channel) {
			ADDENV("REMUX_CHANNEL_ID=%s", *Channel->GetChannelID().ToString());
			ADDENV("REMUX_CHANNEL_NAME=%s", Channel->Name());
			ADDENV("REMUX_VTYPE=%d", Channel->Vtype());
			if (Channel->Vpid())
				ADDENV("REMUX_VPID=%d", Channel->Vpid());
			if (Channel->Ppid() != Channel->Vpid())
				ADDENV("REMUX_PPID=%d", Channel->Ppid());
			if (Channel->Tpid())
				ADDENV("REMUX_TPID=%d", Channel->Tpid());
		}
		else if (PatPmt) {
			ADDENV("REMUX_VTYPE=%d", PatPmt->Vtype());
			if (PatPmt->Vpid())
				ADDENV("REMUX_VPID=%d", PatPmt->Vpid());
			if (PatPmt->Ppid() != PatPmt->Vpid())
				ADDENV("REMUX_PPID=%d", PatPmt->Ppid());
		}

		std::string buffer;
		if (Apids && *Apids) {
			for (const int *pid = Apids; *pid; pid++)
				(buffer += (const char *) itoa(*pid)) += (*(pid + 1) ? " " : "");
			ADDENV("REMUX_APID=%s", buffer.c_str());

			buffer.clear();
			for (const int *pid = Apids; *pid; pid++) {
				int j;
				if (Channel) {
					for (j = 0; Channel->Apid(j) && Channel->Apid(j) != *pid; j++)
						;
					(buffer += Channel->Alang(j)) += (*(pid + 1) ? " " : "");
				}
				else if (PatPmt) {
					for (j = 0; PatPmt->Apid(j) && PatPmt->Apid(j) != *pid; j++)
						;
					(buffer += PatPmt->Alang(j)) += (*(pid + 1) ? " " : "");
				}
			}
			ADDENV("REMUX_ALANG=%s", buffer.c_str());
		}

		if (Dpids && *Dpids) {
			buffer.clear();
			for (const int *pid = Dpids; *pid; pid++)
				(buffer += (const char *) itoa(*pid)) += (*(pid + 1) ? " " : "");
			ADDENV("REMUX_DPID=%s", buffer.c_str());

			buffer.clear();
			for (const int *pid = Dpids; *pid; pid++) {
				int j;
				if (Channel) {
					for (j = 0; Channel->Dpid(j) && Channel->Dpid(j) != *pid; j++)
						;
					(buffer += Channel->Dlang(j)) += (*(pid + 1) ? " " : "");
				}
				else if (PatPmt) {
					for (j = 0; PatPmt->Dpid(j) && PatPmt->Dpid(j) != *pid; j++)
						;
					(buffer += PatPmt->Dlang(j)) += (*(pid + 1) ? " " : "");
				}
			}
			ADDENV("REMUX_DLANG=%s", buffer.c_str());
		}

		if (Channel && Channel->Spid(0)) {
			buffer.clear();
			for (const int *pid = Channel->Spids(); *pid; pid++)
				(buffer += (const char *) itoa(*pid)) += (*(pid + 1) ? " " : "");
			ADDENV("REMUX_SPID=%s", buffer.c_str());

			buffer.clear();
			for (int j = 0; Channel->Spid(j); j++)
				(buffer += Channel->Slang(j)) += (Channel->Spid(j + 1) ? " " : "");
			ADDENV("REMUX_SLANG=%s", buffer.c_str());
		}
		else if (PatPmt && PatPmt->Spid(0)) {
			buffer.clear();
			for (const int *pid = PatPmt->Spids(); *pid; pid++)
				(buffer += (const char *) itoa(*pid)) += (*(pid + 1) ? " " : "");
			ADDENV("REMUX_SPID=%s", buffer.c_str());

			buffer.clear();
			for (int j = 0; PatPmt->Spid(j); j++)
				(buffer += PatPmt->Slang(j)) += (PatPmt->Spid(j + 1) ? " " : "");
			ADDENV("REMUX_SLANG=%s", buffer.c_str());
		}

		if (Connection) {
			// add vars for a CGI like interface
			// the following vars are not implemented:
			// REMOTE_HOST, REMOTE_IDENT, REMOTE_USER
			// CONTENT_TYPE, CONTENT_LENGTH,
			// SCRIPT_NAME, PATH_TRANSLATED, GATEWAY_INTERFACE
			ADDENV("REMOTE_ADDR=%s", Connection->RemoteIp().c_str());
			ADDENV("SERVER_NAME=%s", Connection->LocalIp().c_str());
			ADDENV("SERVER_PORT=%d", Connection->LocalPort());
			ADDENV("SERVER_PROTOCOL=%s", Connection->Protocol());
			ADDENV("SERVER_SOFTWARE=%s", VERSION);

			for (tStrStrMap::const_iterator it = Connection->Headers().begin(); it != Connection->Headers().end(); ++it) {
				if (i >= MAXENV) {
					esyslog("streamdev-server: Too many headers for externremux.sh");
					break;
				}
				ADDENV("%s=%s", it->first.c_str(), it->second.c_str());
			}

			// look for section parameters: /path;param1=value1;param2=value2/
			std::string::size_type begin, end;
			const static std::string PATH_INFO("PATH_INFO");

			tStrStrMap::const_iterator it_pathinfo = Connection->Headers().find(PATH_INFO);
			const std::string& path = it_pathinfo == Connection->Headers().end() ? "/" : it_pathinfo->second;
			begin = path.find(';', 0);
			begin = path.find_first_not_of(';', begin);
			end = path.find_first_of(";/", begin);
			while (begin != std::string::npos && path[begin] != '/') {
				std::string param = path.substr(begin, end - begin);
				std::string::size_type e = param.find('=');
	
				if (i >= MAXENV) {
					esyslog("streamdev-server: Too many parameters for externremux.sh");
					break;
				}
				else if (e > 0 && e != std::string::npos) {
					ADDENV("REMUX_PARAM_%s", param.c_str());
				}
				else
					esyslog("streamdev-server: Invalid externremux.sh parameter %s", param.c_str());
	
				begin = path.find_first_not_of(';', end);
				end = path.find_first_of(";/", begin);
			}
		}
			
		env[i] = NULL;

		dup2(inpipe[0], STDIN_FILENO);
		close(inpipe[1]);
		dup2(outpipe[1], STDOUT_FILENO);
		close(outpipe[0]);

		int MaxPossibleFileDescriptors = getdtablesize();
		for (int i = STDERR_FILENO + 1; i < MaxPossibleFileDescriptors; i++)
			close(i); //close all dup'ed filedescriptors

		if (setpgid(0, 0) == -1)
			esyslog("streamdev-server: externremux setpgid failed: %m");

		if (access(opt_remux, X_OK) == -1) {
			esyslog("streamdev-server %s: %m", opt_remux);
			_exit(-1);
		}

		if (execle("/bin/sh", "sh", "-c", opt_remux, NULL, env) == -1) {
			esyslog("streamdev-server: externremux script '%s' execution failed: %m", opt_remux);
			_exit(-1);
		}
		// should never be reached
		_exit(0);
	}

	close(inpipe[0]);
	close(outpipe[1]);
	m_Inpipe = inpipe[1];
	m_Outpipe = outpipe[0];
	Start();
}

cTSExt::~cTSExt()
{
	m_Active = false;
	Cancel(3);
	if (m_Process > 0) {
		// close pipes
		close(m_Outpipe);
		close(m_Inpipe);
		// signal and wait for termination
		if (kill(m_Process, SIGINT) < 0) {
			esyslog("streamdev-server: externremux SIGINT failed: %m");
		}
		else {
			int i = 0;
			int retval;
			while ((retval = waitpid(m_Process, NULL, WNOHANG)) == 0) {

				if ((++i % 20) == 0) {
					esyslog("streamdev-server: externremux process won't stop - killing it");
					kill(m_Process, SIGKILL);
				}
				cCondWait::SleepMs(100);
			}

			if (retval < 0)
				esyslog("streamdev-server: externremux process waitpid failed: %m");
			else
				Dprintf("streamdev-server: externremux child (%d) exited as expected\n", m_Process);
		}
		m_Process = -1;
	}
}

void cTSExt::Action(void)
{
	m_Active = true;
	while (m_Active) {
		fd_set rfds;
		struct timeval tv;

		FD_ZERO(&rfds);
		FD_SET(m_Outpipe, &rfds);

		while (FD_ISSET(m_Outpipe, &rfds)) {
			tv.tv_sec = 2;
			tv.tv_usec = 0;
			if (select(m_Outpipe + 1, &rfds, NULL, NULL, &tv) == -1) {
				LOG_ERROR_STR("poll failed");
				break;;
			}

			if (FD_ISSET(m_Outpipe, &rfds)) {
				int result;
				//Read returns 0 if buffer full or EOF
				bool bufferFull = m_ResultBuffer->Free() <= 0; //Free may be < 0
				while ((result = m_ResultBuffer->Read(m_Outpipe)) == 0 && bufferFull)
					dsyslog("streamdev-server: buffer full while reading from externremux");

				if (result == -1) {
					if (errno != EINTR && errno != EAGAIN) {
						LOG_ERROR_STR("read failed");
						m_Active = false;
					}
					break;
				}
				else if (result == 0) {
					esyslog("streamdev-server: EOF reading from externremux");
					m_Active = false;
					break;
				}
			}
		}
	}
	m_Active = false;
}


void cTSExt::Put(const uchar *Data, int Count)
{
	if (safe_write(m_Inpipe, Data, Count) == -1) {
		LOG_ERROR_STR("write failed");
		return;
	}
}

cExternRemux::cExternRemux(const cServerConnection *Connection, const cChannel *Channel, const int *Apids, const int *Dpids):
		m_ResultBuffer(new cRingBufferLinear(WRITERBUFSIZE)),
		m_Remux(new cTSExt(m_ResultBuffer, Connection, Channel, NULL, Apids, Dpids))
{
	m_ResultBuffer->SetTimeouts(500, 100);
}
cExternRemux::cExternRemux(const cServerConnection *Connection, const cPatPmtParser *PatPmt, const int *Apids, const int *Dpids):
		m_ResultBuffer(new cRingBufferLinear(WRITERBUFSIZE)),
		m_Remux(new cTSExt(m_ResultBuffer, Connection, NULL, PatPmt, Apids, Dpids))
{
	m_ResultBuffer->SetTimeouts(500, 100);
}

cExternRemux::~cExternRemux()
{
	delete m_Remux;
	delete m_ResultBuffer;
}

int cExternRemux::Put(const uchar *Data, int Count) 
{
	m_Remux->Put(Data, Count);
	return Count;
}
