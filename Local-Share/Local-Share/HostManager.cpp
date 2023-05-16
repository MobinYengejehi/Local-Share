#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include "HostManager.h"

#define SOCKET_MAX_CONNECTIONS 1
#define DEFAULT_SOCKET_BUFFER_LENGTH 0xfff

#define SOCKET_CLIENT_CONNECT_MESSAGE_TYPE "connect"
#define SOCKET_CLIENT_DISCONNECT_MESSAGE_TYPE "disconnect"

#define SOCKET_CLIENT_DISCONNECT_MESSAGE "disconnected"
#define SOCKET_CLIENT_TIMEOUT_MESSAGE "Timed out"
#define SOCKET_CLIENT_CONNECTION_CLOSE_MESSAGE "connection closed"

#define SOCKET_CLIENT_DISCONNECT_REQUEST_MESSAGE "requestdc"

HostManager::HostManager() {
	running = false;
	connected = false;

	port = -1;
	server = INVALID_SOCKET;
	client = INVALID_SOCKET;

	readBuffer = NULL;
	resultRecieved = NULL;

	totalBytesRecieved = 0;

	SetBufferLength(DEFAULT_SOCKET_BUFFER_LENGTH);
}

HostManager::~HostManager() {
	if (readBuffer) {
		realloc(readBuffer, 0);
		readBuffer = NULL;
	}
	if (resultRecieved) {
		realloc(resultRecieved, 0);
		resultRecieved = NULL;
	}

	Stop();
}

std::string& HostManager::GetServerHost() {
	return host;
}

int& HostManager::GetServerPort() {
	return port;
}

EventManager& HostManager::GetEventManager() {
	return events;
}

SOCKET& HostManager::GetServerSocket() {
	return server;
}

SOCKET& HostManager::GetClientSocket() {
	return client;
}

size_t& HostManager::GetBufferLength() {
	return bufferLength;
}

char* HostManager::GetReadBuffer() {
	return readBuffer;
}

char* HostManager::GetResultRecieved() {
	return resultRecieved;
}

long& HostManager::GetTotalBytesRecieved() {
	return totalBytesRecieved;
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

void HostManager::SetBufferLength(const size_t& length) {
	bufferLength = length;

	readBuffer = (char*)realloc(readBuffer, length);
	memset(readBuffer, 0, length);
}

void HostManager::TriggerClientEvent(SocketData& data) {
	if (data.type.empty()) {
		return;
	}

	if (!running) {
		return;
	}
	if (!connected) {
		return;
	}

	std::string dataBuffer = data;
	
	if (send(client, dataBuffer.c_str(), dataBuffer.length(), NULL) == SOCKET_ERROR) {
		std::string error = "send failed with error : ";
		error += std::to_string(WSAGetLastError());

		throw std::exception(error.c_str());
	}
}

void HostManager::Start() {
	if (running) {
		throw std::exception("server is already running");
	}

	WSADATA wsaData;

	struct addrinfo* result = NULL;
	struct addrinfo  hints;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::string error = "WSAStartup failed with error : ";
		error += std::to_string(WSAGetLastError());

		throw std::exception(error.c_str());
	}

	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &result) != 0) {
		CleanUp();
		
		std::string error = "getaddrinfo failed with error : ";
		error += std::to_string(WSAGetLastError());

		throw std::exception(error.c_str());
	}

	server = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (server == INVALID_SOCKET) {
		freeaddrinfo(result);
		CleanUp();

		std::string error = "creating socket failed with error : ";
		error += std::to_string(WSAGetLastError());
		
		throw std::exception(error.c_str());
	}

	if (bind(server, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
		freeaddrinfo(result);
		CleanUp();

		std::string error = "binding socket failed with error : ";
		error += std::to_string(WSAGetLastError());

		throw std::exception(error.c_str());
	}

	freeaddrinfo(result);

	if (listen(server, SOCKET_MAX_CONNECTIONS) == SOCKET_ERROR) {
		CleanUp();

		std::string error = "socket listening failed with error : ";
		error += std::to_string(WSAGetLastError());

		throw std::exception(error.c_str());
	}

	running = true;
}

void HostManager::Stop() {
	if (!running) {
		throw std::exception("server isn't running");
	}

	if (IsConnected()) {
		DisconnectClient();
	}
	
	closesocket(server);
	server = INVALID_SOCKET;
	running = false;

	CleanUp();
}

void HostManager::ProcessConnection() {
	if (!running) {
		return;
	}
	
	if (!connected) {
		struct sockaddr_in clientAddress;
		int addressLength = sizeof clientAddress;

		client = accept(server, (struct sockaddr*)&clientAddress, (socklen_t*)&addressLength);
		if (client == INVALID_SOCKET) {
			return;
		}

		connected = true;

		SocketData data;
		data.type = SOCKET_CLIENT_CONNECT_MESSAGE_TYPE;
		events.Trigger(data);
		return;
	}

	memset(readBuffer, 0, bufferLength);

	int readSize = recv(client, readBuffer, bufferLength, 0);
	if (readSize <= SOCKET_ERROR) {
		closesocket(client);
		client = INVALID_SOCKET;
		connected = false;

		SocketData data;
		data.type = SOCKET_CLIENT_DISCONNECT_MESSAGE_TYPE;
		data.entries.push_back({"reason", SOCKET_CLIENT_TIMEOUT_MESSAGE});
		events.Trigger(data);
		return;
	}

	resultRecieved = (char*)realloc(resultRecieved, totalBytesRecieved + readSize);
	memcpy(resultRecieved + totalBytesRecieved, readBuffer, readSize);

	totalBytesRecieved += readSize;

	if (readSize < bufferLength) {
		if (resultRecieved == SOCKET_CLIENT_DISCONNECT_REQUEST_MESSAGE) {
			closesocket(client);
			client = INVALID_SOCKET;
			connected = false;

			SocketData data;
			data.type = SOCKET_CLIENT_DISCONNECT_MESSAGE_TYPE;
			data.entries.push_back({ "reason", SOCKET_CLIENT_DISCONNECT_MESSAGE });
			events.Trigger(data);
			return;
		}

		SocketData data = std::string(resultRecieved, totalBytesRecieved);
		events.Trigger(data);

		realloc(resultRecieved, 0);
		resultRecieved = NULL;

		totalBytesRecieved = 0;
	}
}

void HostManager::DisconnectClient() {
	if (!connected) {
		throw std::exception("server doens't have any client connections");
	}

	SocketData data;
	data.type = SOCKET_CLIENT_DISCONNECT_MESSAGE_TYPE;
	data.entries.push_back({"reason", SOCKET_CLIENT_CONNECTION_CLOSE_MESSAGE});
	events.Trigger(data);

	const char* closeRequset = SOCKET_CLIENT_DISCONNECT_REQUEST_MESSAGE;
	send(client, closeRequset, strlen(closeRequset), 0);

	closesocket(client);
	connected = false;
	client = INVALID_SOCKET;
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