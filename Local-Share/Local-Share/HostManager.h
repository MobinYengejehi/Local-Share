#pragma once

#ifndef HOST_MANAGER_HEADER
#define HOST_MANAGER_HEADER

#include <string>
#include <vector>
#include <map>
#include <ws2tcpip.h>

#include "SocketData.h"
#include "EventManager.h"

#define SOCKET_MAX_CONNECTIONS 1
#define DEFAULT_SOCKET_BUFFER_LENGTH 0xfff

#define SOCKET_CLIENT_CONNECT_MESSAGE_TYPE "connect"
#define SOCKET_CLIENT_DISCONNECT_MESSAGE_TYPE "disconnect"

#define SOCKET_CLIENT_DISCONNECT_MESSAGE "disconnected"
#define SOCKET_CLIENT_TIMEOUT_MESSAGE "Timed out"
#define SOCKET_CLIENT_CONNECTION_CLOSE_MESSAGE "connection closed"

#define SOCKET_CLIENT_DISCONNECT_REQUEST_MESSAGE "requestdc"

class HostManager {
public:
	HostManager();
	~HostManager();

	std::string& GetServerHost();
	int&         GetServerPort();

	EventManager& GetEventManager();

	SOCKET& GetServerSocket();
	SOCKET& GetClientSocket();

	size_t& GetBufferLength();

	char* GetReadBuffer();
	char* GetResultRecieved();

	long& GetTotalBytesRecieved();

	void SetServerHost(const std::string&);
	void SetServerPort(const int&);

	void SetServerSocket(const SOCKET&);
	void SetClientSocket(const SOCKET&);

	void SetBufferLength(const size_t&);

	void TriggerClientEvent(SocketData&);

	void Start();
	void Stop();

	void ProcessConnection();

	void DisconnectClient();

	bool IsRunning();
	bool IsConnected();

	static void CleanUp();
private:
	std::string host;
	int         port;
	bool        running;
	bool        connected;

	EventManager events;

	SOCKET server;
	SOCKET client;

	size_t bufferLength;

	char* readBuffer;
	char* resultRecieved;

	long totalBytesRecieved;
};

#endif