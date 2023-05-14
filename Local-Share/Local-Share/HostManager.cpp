#include "HostManager.h"

HostManager::HostManager() {
	running = false;
	connected = false;

	port = -1;
	server = -1;
	client = -1;
}

HostManager::~HostManager() {
	Stop();
}

std::string& HostManager::GetServerHost() {
	return host;
}

int& HostManager::GetServerPort() {
	return port;
}

SOCKET& HostManager::GetServerSocket() {
	return server;
}

SOCKET& HostManager::GetClientSocket() {
	return client;
}

void HostManager::SetServerHost(const std::string& ip) {
	host = ip;
}

void HostManager::SetServerPort(const int& portNumber) {
	port = portNumber;
}

void HostManager::SetServerSocket(const SOCKET& serverSocket) {
	server = serverSocket;
}

void HostManager::SetClientSocket(const SOCKET& clientSocket) {
	client = clientSocket;
}

void HostManager::Start() {

}

void HostManager::Stop() {
	if (!running) {
		throw std::exception("server isn't running");
	}

	if (IsConnected()) {
		DisconnectClient();
	}
	
	closesocket(server);
	server = -1;
	running = false;
}

void HostManager::ProcessConnection() {
	
}

void HostManager::DisconnectClient() {
	if (!connected) {
		throw std::exception("server doens't have any client connections");
	}

	closesocket(client);
	connected = false;
	client = -1;
}

bool HostManager::IsRunning() {
	return running;
}

bool HostManager::IsConnected() {
	return connected;
}

void HostManager::CleanUp() {
	WSACleanup();
}