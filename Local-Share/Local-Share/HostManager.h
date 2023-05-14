#pragma once

#ifndef HOST_MANAGER_HEADER
#define HOST_MANAGER_HEADER

#include <string>
#include <Windows.h>

class HostManager {
public:
	HostManager() = default;
	~HostManager() = default;

	std::string& GetServerHost();
	int&         GetServerPort();

	SOCKET& GetServerSocket();
	SOCKET& GetClientSocket();

	void SetServerHost(const std::string&);
	void SetServerPort(const int&);

	void SetServerSocket(const SOCKET&);
	void SetClientSocket(const SOCKET&);

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
	bool        connected;

	SOCKET server;
	SOCKET client;
};

#endif