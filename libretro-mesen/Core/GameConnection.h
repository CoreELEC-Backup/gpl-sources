#pragma once
#include "stdafx.h"
#include "../Utilities/SimpleLock.h"

class Socket;
class NetMessage;
class Console;

struct PlayerInfo
{
	string Name;
	uint8_t ControllerPort;
	bool IsHost;
};

class GameConnection
{
protected:
	shared_ptr<Socket> _socket;
	shared_ptr<Console> _console;

	uint8_t _readBuffer[0x40000] = {};
	uint8_t _messageBuffer[0x40000] = {};
	int _readPosition = 0;
	SimpleLock _socketLock;

private:
	void ReadSocket();

	bool ExtractMessage(void *buffer, uint32_t &messageLength);
	NetMessage* ReadMessage();

	virtual void ProcessMessage(NetMessage* message) = 0;

protected:
	void Disconnect();

public:
	static constexpr uint8_t SpectatorPort = 0xFF;
	GameConnection(shared_ptr<Console> console, shared_ptr<Socket> socket);

	bool ConnectionError();
	void ProcessMessages();
	void SendNetMessage(NetMessage &message);
};