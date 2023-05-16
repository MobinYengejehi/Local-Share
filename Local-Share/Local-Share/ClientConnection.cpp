#include "ClientConnection.h"

#define CLIENT_CONNECT_MESSAGE_TYPE "connect"
#define CLIENT_DISCONNECT_MESSAGE_TYPE "disconnect"

#define CLIENT_DISCONNECT_REQUEST_MESSAGE "requestdc"

#define DEFAULT_CLIENT_BUFFER_LENGTH 0xfff

ClientConnection::ClientConnection() {
	connected = false;

	port = -1;
	socket = INVALID_SOCKET;

	readBuffer = NULL;
	resultRecieved = NULL;

	totalBytesRecieved = 0;

	SetBufferLength(DEFAULT_CLIENT_BUFFER_LENGTH);
}

ClientConnection::~ClientConnection() {
	if (readBuffer) {
		realloc(readBuffer, 0);
		readBuffer = NULL;
	}
	if (resultRecieved) {
		realloc(resultRecieved, 0);
		resultRecieved = NULL;
	}

	Disconnect();
}

std::string& ClientConnection::GetServerHost() {
	return host;
}

int& ClientConnection::GetServerPort() {
	return port;
}

EventManager& ClientConnection::GetEventManager() {
	return events;
}

SOCKET& ClientConnection::GetSocket() {
	return socket;
}

size_t& ClientConnection::GetBufferLength() {
	return bufferLength;
}

char* ClientConnection::GetReadBuffer() {
	return readBuffer;
}

char* ClientConnection::GetResultRecieved() {
	return resultRecieved;
}

long& ClientConnection::GetTotalBytesRecieved() {
	return totalBytesRecieved;
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

void ClientConnection::SetBufferLength(const size_t& length) {
	bufferLength = length;

	readBuffer = (char*)realloc(readBuffer, length);
	memset(readBuffer, 0, length);
}

void ClientConnection::TriggerServerEvent(SocketData& data) {
	if (data.type.empty()) {
		return;
	}

	if (!connected) {
		return;
	}

	std::string dataBuffer = data;

	if (send(socket, dataBuffer.c_str(), dataBuffer.length(), NULL) == SOCKET_ERROR) {
		std::string error = "send failed with error : ";
		error += std::to_string(WSAGetLastError());

		throw std::exception(error.c_str());
	}
}

void ClientConnection::Connect() {
	if (connected) {
		throw std::exception("already connected to server");
	}
	if (host.empty()) {
		throw std::exception("invalid host address");
	}

	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		std::string error = "WSAStartup failed with error : ";
		error += WSAGetLastError();

		throw std::exception(error.c_str());
	}

	socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket == INVALID_SOCKET) {
		CleanUp();
		throw std::exception("couldn't create socket");
	}

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(host.c_str());
	server.sin_port = htons(port);

	if (connect(socket, (SOCKADDR*)&server, sizeof server) == SOCKET_ERROR) {
		closesocket(socket);
		CleanUp();

		throw std::exception("couldn't connect to server");
	}

	connected = true;

	SocketData data;
	data.type = CLIENT_CONNECT_MESSAGE_TYPE;
	events.Trigger(data);
}

void ClientConnection::Disconnect() {
	if (!connected) {
		throw std::exception("didn't connected to any data share server");
	}

	SocketData data;
	data.type = CLIENT_DISCONNECT_MESSAGE_TYPE;
	events.Trigger(data);

	const char* closeRequset = CLIENT_DISCONNECT_REQUEST_MESSAGE;
	send(socket, closeRequset, strlen(closeRequset), 0);

	closesocket(socket);
	connected = false;
	socket = INVALID_SOCKET;
}

void ClientConnection::ProcessConnection() {
	if (!connected) {
		return;
	}

	memset(readBuffer, 0, bufferLength);

	int readSize = recv(socket, readBuffer, bufferLength, 0);
	if (readSize <= SOCKET_ERROR) {
		closesocket(socket);
		socket = INVALID_SOCKET;
		connected = false;

		SocketData data;
		data.type = CLIENT_DISCONNECT_MESSAGE_TYPE;
		events.Trigger(data);
		return;
	}

	resultRecieved = (char*)realloc(resultRecieved, totalBytesRecieved + readSize);
	memcpy(resultRecieved + totalBytesRecieved, readBuffer, readSize);

	totalBytesRecieved += readSize;

	if (readSize < bufferLength) {
		if (resultRecieved == CLIENT_DISCONNECT_REQUEST_MESSAGE) {
			closesocket(socket);
			socket = INVALID_SOCKET;
			connected = false;

			SocketData data;
			data.type = CLIENT_DISCONNECT_MESSAGE_TYPE;
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

bool ClientConnection::IsConnected() {
	return connected;
}

void ClientConnection::CleanUp() {
	WSACleanup();
}