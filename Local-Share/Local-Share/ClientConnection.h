#pragma once

#ifndef CLIENT_CONNECTION_HEADER
#define CLIENT_CONNECTION_HEADER

#include <string>
#include <Windows.h>

class ClientConnection {
public:
	ClientConnection();
	~ClientConnection();

	std::string& GetServerHost();
	int&         GetServerPort();
	SOCKET&      GetSocket();

	void SetServerHost(const std::string&);
	void SetServerPort(const int&);
	void SetSocket(const SOCKET&);

	void Connect();
	void Disconnect();

	bool IsConnected();
private:
	std::string host;
	int         port;
	bool        connected;

	SOCKET      socket;
};

#endif