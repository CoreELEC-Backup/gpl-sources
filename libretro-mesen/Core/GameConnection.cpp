#include "stdafx.h"
#include "GameConnection.h"
#include "HandShakeMessage.h"
#include "InputDataMessage.h"
#include "MovieDataMessage.h"
#include "GameInformationMessage.h"
#include "SaveStateMessage.h"
#include "PlayerListMessage.h"
#include "SelectControllerMessage.h"
#include "ClientConnectionData.h"
#include "ForceDisconnectMessage.h"
#include "ServerInformationMessage.h"

GameConnection::GameConnection(shared_ptr<Console> console, shared_ptr<Socket> socket)
{
	_console = console;
	_socket = socket;
}

void GameConnection::ReadSocket()
{
	auto lock = _socketLock.AcquireSafe();
	int bytesReceived = _socket->Recv((char*)_readBuffer + _readPosition, 0x40000 - _readPosition, 0);
	if(bytesReceived > 0) {
		_readPosition += bytesReceived;
	}
}

bool GameConnection::ExtractMessage(void *buffer, uint32_t &messageLength)
{
	messageLength = _readBuffer[0] | (_readBuffer[1] << 8) | (_readBuffer[2] << 16) | (_readBuffer[3] << 24);

	if(messageLength > 1000000) {
		MessageManager::Log("[Netplay] Invalid data received, closing connection.");
		Disconnect();
		return false;
	}

	int packetLength = messageLength + sizeof(messageLength);

	if(_readPosition >= packetLength) {
		memcpy(buffer, _readBuffer+sizeof(messageLength), messageLength);
		memmove(_readBuffer, _readBuffer + packetLength, _readPosition - packetLength);
		_readPosition -= packetLength;
		return true;
	}
	return false;
}

NetMessage* GameConnection::ReadMessage()
{
	ReadSocket();

	if(_readPosition > 4) {
		uint32_t messageLength;
		if(ExtractMessage(_messageBuffer, messageLength)) {
			switch((MessageType)_messageBuffer[0]) {
				case MessageType::HandShake: return new HandShakeMessage(_messageBuffer, messageLength);
				case MessageType::SaveState: return new SaveStateMessage(_messageBuffer, messageLength);
				case MessageType::InputData: return new InputDataMessage(_messageBuffer, messageLength);
				case MessageType::MovieData: return new MovieDataMessage(_messageBuffer, messageLength);
				case MessageType::GameInformation: return new GameInformationMessage(_messageBuffer, messageLength);
				case MessageType::PlayerList: return new PlayerListMessage(_messageBuffer, messageLength);
				case MessageType::SelectController: return new SelectControllerMessage(_messageBuffer, messageLength);
				case MessageType::ForceDisconnect: return new ForceDisconnectMessage(_messageBuffer, messageLength);
				case MessageType::ServerInformation: return new ServerInformationMessage(_messageBuffer, messageLength);
			}
		}
	}
	return nullptr;
}

void GameConnection::SendNetMessage(NetMessage &message)
{
	auto lock = _socketLock.AcquireSafe();
	message.Send(*_socket.get());
}

void GameConnection::Disconnect()
{
	auto lock = _socketLock.AcquireSafe();
	_socket->Close();
}

bool GameConnection::ConnectionError()
{
	return _socket->ConnectionError();
}

void GameConnection::ProcessMessages()
{
	NetMessage* message;
	while((message = ReadMessage()) != nullptr) {
		//Loop until all messages have been processed
		message->Initialize();
		ProcessMessage(message);
		delete message;
	}		
}