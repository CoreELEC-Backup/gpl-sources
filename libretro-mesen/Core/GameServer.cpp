#include "stdafx.h"
#include <thread>
using std::thread;

#include "MessageManager.h"
#include "GameServer.h"
#include "Console.h"
#include "ControlManager.h"
#include "../Utilities/Socket.h"
#include "PlayerListMessage.h"
#include "NotificationManager.h"

shared_ptr<GameServer> GameServer::Instance;

GameServer::GameServer(shared_ptr<Console> console, uint16_t listenPort, string password, string hostPlayerName)
{
	_console = console;
	_stop = false;
	_port = listenPort;
	_password = password;
	_hostPlayerName = hostPlayerName;
	_hostControllerPort = 0;

	//If a game is already running, register ourselves as an input recorder/provider right away
	RegisterServerInput();
}

GameServer::~GameServer()
{
	_stop = true;
	_serverThread->join();

	Stop();

	ControlManager* controlManager = _console->GetControlManager();
	if(controlManager) {
		controlManager->UnregisterInputRecorder(this);
		controlManager->UnregisterInputProvider(this);
	}
}

void GameServer::RegisterServerInput()
{
	ControlManager* controlManager = _console->GetControlManager();
	if(controlManager) {
		controlManager->RegisterInputRecorder(this);
		controlManager->RegisterInputProvider(this);
	}
}

void GameServer::AcceptConnections()
{
	while(true) {
		shared_ptr<Socket> socket = _listener->Accept();
		if(!socket->ConnectionError()) {
			auto connection = shared_ptr<GameServerConnection>(new GameServerConnection(_console, socket, _password));
			_console->GetNotificationManager()->RegisterNotificationListener(connection);
			_openConnections.push_back(connection);
		} else {
			break;
		}
	}
	_listener->Listen(10);
}

void GameServer::UpdateConnections()
{
	vector<shared_ptr<GameServerConnection>> connectionsToRemove;
	for(shared_ptr<GameServerConnection> connection : _openConnections) {
		if(connection->ConnectionError()) {
			connectionsToRemove.push_back(connection);
		} else {
			connection->ProcessMessages();
		}
	}

	for(shared_ptr<GameServerConnection> gameConnection : connectionsToRemove) {
		_openConnections.remove(gameConnection);
	}
}

list<shared_ptr<GameServerConnection>> GameServer::GetConnectionList()
{
	if(GameServer::Started()) {
		return Instance->_openConnections;
	} else {
		return list<shared_ptr<GameServerConnection>>();
	}
}

bool GameServer::SetInput(BaseControlDevice *device)
{
	uint8_t port = device->GetPort();

	GameServerConnection* connection = GameServerConnection::GetNetPlayDevice(port);
	if(connection) {
		//Device is controlled by a client
		device->SetRawState(connection->GetState());
		return true;
	}

	//Host is controlling this device
	return false;
}

void GameServer::RecordInput(vector<shared_ptr<BaseControlDevice>> devices)
{
	for(shared_ptr<BaseControlDevice> &device : devices) {
		for(shared_ptr<GameServerConnection> connection : _openConnections) {
			if(!connection->ConnectionError()) {
				//Send movie stream
				connection->SendMovieData(device->GetPort(), device->GetRawState());
			}
		}
	}
}

void GameServer::ProcessNotification(ConsoleNotificationType type, void * parameter)
{
	if(type == ConsoleNotificationType::GameLoaded) {
		//Register the server as an input provider/recorder
		RegisterServerInput();
	}
}

void GameServer::Exec()
{
	_listener.reset(new Socket());
	_listener->Bind(_port);
	_listener->Listen(10);
	_stop = false;
	_initialized = true;
	MessageManager::DisplayMessage("NetPlay" , "ServerStarted", std::to_string(_port));

	while(!_stop) {
		AcceptConnections();
		UpdateConnections();

		std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(1));
	}
}

void GameServer::Stop()
{
	_initialized = false;
	_listener.reset();
	MessageManager::DisplayMessage("NetPlay", "ServerStopped");
}

void GameServer::StartServer(shared_ptr<Console> console, uint16_t port, string password, string hostPlayerName)
{
	Instance.reset(new GameServer(console, port, password, hostPlayerName));
	console->GetNotificationManager()->RegisterNotificationListener(Instance);
	Instance->_serverThread.reset(new thread(&GameServer::Exec, Instance.get()));
}

void GameServer::StopServer()
{
	if(Instance) {
		Instance.reset();
	}
}

bool GameServer::Started()
{
	if(Instance) {
		return Instance->_initialized;
	} else {
		return false;
	}
}

string GameServer::GetHostPlayerName()
{
	if(GameServer::Started()) {
		return Instance->_hostPlayerName;
	}
	return "";
}

uint8_t GameServer::GetHostControllerPort()
{
	if(GameServer::Started()) {
		return Instance->_hostControllerPort;
	}
	return GameConnection::SpectatorPort;
}

void GameServer::SetHostControllerPort(uint8_t port)
{
	if(GameServer::Started()) {
		Instance->_console->Pause();
		if(port == GameConnection::SpectatorPort || GetAvailableControllers() & (1 << port)) {
			//Port is available
			Instance->_hostControllerPort = port;
			SendPlayerList();
		}
		Instance->_console->Resume();
	}
}

uint8_t GameServer::GetAvailableControllers()
{
	uint8_t availablePorts = (1 << BaseControlDevice::PortCount) - 1;
	for(PlayerInfo &playerInfo : GetPlayerList()) {
		if(playerInfo.ControllerPort < BaseControlDevice::PortCount) {
			availablePorts &= ~(1 << playerInfo.ControllerPort);
		}
	}
	return availablePorts;
}

vector<PlayerInfo> GameServer::GetPlayerList()
{
	vector<PlayerInfo> playerList;

	PlayerInfo playerInfo;
	playerInfo.Name = GetHostPlayerName();
	playerInfo.ControllerPort = GetHostControllerPort();
	playerInfo.IsHost = true;
	playerList.push_back(playerInfo);

	for(shared_ptr<GameServerConnection> &connection : GetConnectionList()) {
		playerInfo.Name = connection->GetPlayerName();
		playerInfo.ControllerPort = connection->GetControllerPort();
		playerInfo.IsHost = false;
		playerList.push_back(playerInfo);
	}

	return playerList;
}

void GameServer::SendPlayerList()
{
	vector<PlayerInfo> playerList = GetPlayerList();

	for(shared_ptr<GameServerConnection> &connection : GetConnectionList()) {
		//Send player list update to all connections
		PlayerListMessage message(playerList);
		connection->SendNetMessage(message);
	}
}