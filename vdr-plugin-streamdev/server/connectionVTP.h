#ifndef VDR_STREAMDEV_SERVERS_CONNECTIONVTP_H
#define VDR_STREAMDEV_SERVERS_CONNECTIONVTP_H

#include "server/connection.h"
#include "server/recplayer.h"

class cTBSocket;
class cStreamdevFilterStreamer;
class cLSTEHandler;
class cLSTCHandler;
class cLSTTHandler;
class cLSTRHandler;

class cConnectionVTP: public cServerConnection {
	friend class cLSTEHandler;
#if !defined __GNUC__ || __GNUC__ >= 3
	using cServerConnection::Respond;
#endif

private:
	cTBSocket                *m_LiveSocket;
	cTBSocket                *m_FilterSocket;
	cStreamdevFilterStreamer *m_FilterStreamer;
	cTBSocket                *m_RecSocket;
	cTBSocket                *m_DataSocket;

	char                   *m_LastCommand;
	eStreamType             m_StreamType;
	unsigned int            m_ClientVersion;
	bool                    m_FiltersSupport;
	bool                    m_LoopPrevention;
	RecPlayer              *m_RecPlayer;

	// Priority is only known in PROV command
	// Store in here for later use in TUNE call
	const cChannel         *m_TuneChannel;
	int                     m_TunePriority;

	// Members adopted for SVDRP
	cLSTEHandler *m_LSTEHandler;
	cLSTCHandler *m_LSTCHandler;
	cLSTTHandler *m_LSTTHandler;
	cLSTRHandler *m_LSTRHandler;

protected:
	template<class cHandler>
	bool CmdLSTX(cHandler *&Handler, char *Option);

public:
	cConnectionVTP(void);
	virtual ~cConnectionVTP();

	virtual void Welcome(void);
	virtual void Reject(void);

	virtual cString ToText(char Delimiter = ' ') const;

	virtual bool Abort(void) const;
	virtual void Detach(void);
	virtual void Attach(void);

	virtual bool Command(char *Cmd);
	bool CmdCAPS(char *Opts);
	bool CmdVERS(char *Opts);
	bool CmdPROV(char *Opts);
	bool CmdPORT(char *Opts);
	bool CmdREAD(char *Opts);
	bool CmdTUNE(char *Opts);
	bool CmdPLAY(char *Opts);
	bool CmdPRIO(char *Opts);
	bool CmdSGNL(char *Opts);
	bool CmdADDP(char *Opts);
	bool CmdDELP(char *Opts);
	bool CmdADDF(char *Opts);
	bool CmdDELF(char *Opts);
	bool CmdABRT(char *Opts);
	bool CmdQUIT(void);
	bool CmdSUSP(void);

	// Thread-safe implementations of SVDRP commands
	bool CmdLSTE(char *Opts);
	bool CmdLSTC(char *Opts);
	bool CmdLSTT(char *Opts);
	bool CmdLSTR(char *Opts);

	// Commands adopted from SVDRP
	bool CmdSTAT(const char *Option);
	bool CmdMODT(const char *Option);
	bool CmdNEWT(const char *Option);
	bool CmdDELT(const char *Option);
	bool CmdNEXT(const char *Option);
	bool CmdNEWC(const char *Option);
	bool CmdMODC(const char *Option);
	bool CmdMOVC(const char *Option);
	bool CmdDELC(const char *Option);
	bool CmdDELR(const char *Option);
	bool CmdRENR(const char *Option);

	bool Respond(int Code, const char *Message, ...)
			__attribute__ ((format (printf, 3, 4)));
};

#endif // VDR_STREAMDEV_SERVERS_CONNECTIONVTP_H
