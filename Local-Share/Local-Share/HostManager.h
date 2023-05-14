#pragma once

#ifndef HOST_MANAGER_HEADER
#define HOST_MANAGER_HEADER

#include <string>
#include <Windows.h>

#include "SocketData.h"

typedef void (*HostManagerDataHandler)(SocketData*);

class HostManager {
public:
	HostManager();
	~HostManager();

	std::string& GetServerHost();
	int&         GetServerPort();

	SOCKET& GetServerSocket();
	SOCKET& GetClientSocket();

	void SetServerHost(const std::string&);
	void SetServerPort(const int&);

	void SetServerSocket(const SOCKET&);
	void SetClientSocket(const SOCKET&);

	void AddDataHandler(const std::string&, HostManagerDataHandler);
	void AddDataHandler(HostManagerDataHandler);
	void RemoveDataHandler(const std::string&, HostManagerDataHandler);
	void RemoveDataHandler(HostManagerDataHandler);
	void TriggerDataHandler(const SocketData&);
	void Cancel();

	void Start();
	void Stop();

	void ProcessConnection();

	void DisconnectClient();

	bool IsRunning();
	bool IsConnected();
	bool IsCanceled();

	static void CleanUp();
private:
	std::string host;
	int         port;
	bool        running;
	bool        connected;
	bool        canceled;

	SOCKET server;
	SOCKET client;
};

#endif