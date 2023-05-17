#pragma once

#ifndef CLIENT_CONNECTION_HEADER
#define CLIENT_CONNECTION_HEADER

#include <string>
#include <Windows.h>

#include "SocketData.h"
#include "EventManager.h"

#define CLIENT_CONNECT_MESSAGE_TYPE "connect"
#define CLIENT_DISCONNECT_MESSAGE_TYPE "disconnect"

#define CLIENT_DISCONNECT_REQUEST_MESSAGE "requestdc"

#define DEFAULT_CLIENT_BUFFER_LENGTH 0xfff

class ClientConnection {
public:
	ClientConnection();
	~ClientConnection();

	std::string&  GetServerHost();
	int&          GetServerPort();

	EventManager& GetEventManager();

	SOCKET& GetSocket();

	size_t& GetBufferLength();

	char* GetReadBuffer();
	char* GetResultRecieved();

	long& GetTotalBytesRecieved();

	void SetServerHost(const std::string&);
	void SetServerPort(const int&);
	
	void SetSocket(const SOCKET&);

	void SetBufferLength(const size_t&);

	void TriggerServerEvent(SocketData&);

	void Connect();
	void Disconnect();

	void ProcessConnection();

	bool IsConnected();

	static void CleanUp();
private:
	std::string host;
	int         port;
	bool        connected;

	EventManager events;

	SOCKET      socket;

	size_t bufferLength;

	char* readBuffer;
	char* resultRecieved;

	long totalBytesRecieved;
};

#endif