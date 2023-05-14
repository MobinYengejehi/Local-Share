#include "ClientConnection.h"

ClientConnection::ClientConnection() {
	connected = false;

	port = -1;
	socket = -1;
}

ClientConnection::~ClientConnection() {
	Disconnect();
}

std::string& ClientConnection::GetServerHost() {
	return host;
}

int& ClientConnection::GetServerPort() {
	return port;
}

SOCKET& ClientConnection::GetSocket() {
	return socket;
}

void ClientConnection::SetServerHost(const std::string& ip) {
	host = ip;
}

void ClientConnection::SetServerPort(const int& portNumber) {
	port = portNumber;
}

void ClientConnection::SetSocket(const SOCKET& serverSocket) {
	socket = serverSocket;
}

void ClientConnection::Connect() {
	if (connected) {
		throw std::exception("already connected to server");
	}

}

void ClientConnection::Disconnect() {
	if (!connected) {
		throw std::exception("didn't connected to any data share server");
	}

	closesocket(socket);
	connected = false;
	socket = -1;
}

bool ClientConnection::IsConnected() {
	return connected;
}