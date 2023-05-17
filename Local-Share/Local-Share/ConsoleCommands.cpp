#include "ConsoleCommands.h"

#include <iostream>
#include <thread>
#include <string.h>

#include "Utility.h"

#include "HostManager.h"
#include "ClientConnection.h"
#include "SharedEvents.h"

typedef void (*VoidFunction)();

extern bool ApplicationRunning;

extern CommandControl* g_CommandManager;

extern HostManager*	     g_HostManager;
extern ClientConnection* g_ClientConnection;

extern std::thread* g_ServerThread;
extern std::thread* g_ClientThread;

extern VoidFunction DataShareServerProcess;
extern VoidFunction ClientConnectionProcess;

#define CONSOLE_LINE_MAX_CHARACTERS 0x78

#define Command void ConsoleCommands::
#define Arguments CommandArguments& arguments

std::string  get(Arguments, const int&);
bool         isint(const std::string&);

Command Help(Arguments) {
	SetConsoleColorMode(ConsoleMode::Output);
	if (arguments.empty()) {
		StringMap& commands = g_CommandManager->GetCommandDescriptions();
		if (!commands.empty()) {
			bool firstColumn = true;
			int middle = CONSOLE_LINE_MAX_CHARACTERS / 2;
			for (auto& block : commands) {
				std::cout << block.first;
				if (firstColumn) {
					int size = middle - block.first.length();
					std::string space(size, 0);
					for (int i = 0; i < size; space[i] = ' ', i++);
					std::cout << space;
				}
				else {
					std::cout << std::endl;
				}
				firstColumn != firstColumn;
			}
			std::cout << std::endl;
		}
	}
	else {
		std::string& command = arguments[0];
		if (g_CommandManager->ExistsCommandDescription(command)) {
			std::string description = g_CommandManager->GetCommandDescription(command);
			std::cout << CURRENT_TIME << INFO_MESSAGE << description << std::endl;
		}
		else {
			std::cout << CURRENT_TIME << ERROR_MESSAGE << "there isn't any descriptions for command '" << command << '\'' << std::endl;
		}
	}
	SetConsoleColorMode(ConsoleMode::Input);
}

Command Exit(Arguments) {
	SetConsoleColorMode(ConsoleMode::Output);
	std::cout << CURRENT_TIME << INFO_MESSAGE << "closing connections..." << std::endl;

	std::cout << CURRENT_TIME << INFO_MESSAGE << "closed all connections" << std::endl;
	SetConsoleColorMode(ConsoleMode::Input);

	ApplicationRunning = false;
	exit(0);
}

Command WorkingDir(Arguments) {
	SetConsoleColorMode(ConsoleMode::Output);
	std::string path;
	GetApplicationDirectory(&path);
	if (path.empty()) {
		path = "Unknown";
	}
	std::cout << CURRENT_TIME << INFO_MESSAGE << "working on : \"" << path << '"' << std::endl;
	SetConsoleColorMode(ConsoleMode::Input);
}

Command Adapters(Arguments) {
	SetConsoleColorMode(ConsoleMode::Output);
	
	NetAdapterList list;
	GetNetAdapterList(&list);

	if (list.empty()) {
		std::cout << CURRENT_TIME << ERROR_MESSAGE << "couldn't find any adapters on your system" << std::endl;
	}else{
		std::cout << CURRENT_TIME << INFO_MESSAGE << "adapters are : " << std::endl;

		for (NetAdapter& adapter : list) {
			std::cout << "Adapter : " << adapter.adapter << std::endl;
			std::cout << "Name    : " << adapter.name << std::endl;
			std::cout << "IP      : " << adapter.ip << std::endl;
			std::cout << std::endl;
		}
	}

	SetConsoleColorMode(ConsoleMode::Input);
}

Command HasWifiAdapter(Arguments) {
	SetConsoleColorMode(ConsoleMode::Output);

	std::cout << CURRENT_TIME << INFO_MESSAGE << "your system " << (::HasWifiAdapter() ? "has" : "doesn't have") << " wifi adapter" << std::endl;

	SetConsoleColorMode(ConsoleMode::Input);
}

Command WifiAddress(Arguments) {
	SetConsoleColorMode(ConsoleMode::Output);

	NetAdapter wifi;
	GetWifiAdapter(&wifi);

	if (IsValidNetAdapter(wifi)) {
		std::cout << CURRENT_TIME << INFO_MESSAGE << "wifi adapter info : " << std::endl;
		std::cout << "Adapter : " << wifi.adapter << std::endl;
		std::cout << "Name    : " << wifi.name << std::endl;
		std::cout << "IP      : " << wifi.ip << std::endl;
	}else{
		if (::HasWifiAdapter()) {
			std::cout << CURRENT_TIME << ERROR_MESSAGE << "doesn't have access to wifi adapter" << std::endl;
		}else{
			std::cout << CURRENT_TIME << ERROR_MESSAGE << "system doesn't have wifi adapter" << std::endl;
		}
	}

	SetConsoleColorMode(ConsoleMode::Input);
}

Command StartServer(Arguments) {
	SetConsoleColorMode(ConsoleMode::Output);

	std::string portString = get(arguments, 0);
	if (portString.empty()) {
		std::cout << CURRENT_TIME << ERROR_MESSAGE << "you must enter server port number as first argument of this command" << std::endl;
	}else{
		int port = std::stoi(portString);
		if (!g_HostManager->IsRunning()) {
			if (!g_ClientConnection->IsConnected()) {
				try {
					g_HostManager->SetServerPort(port);
					g_HostManager->Start();

					if (g_ServerThread) {
						TerminateThread(g_ServerThread->native_handle(), 0);
						g_ServerThread->detach();

						delete g_ServerThread;
						g_ServerThread = NULL;
					}
					
					g_ServerThread = new std::thread(DataShareServerProcess);
					if (g_ServerThread) {
						std::cout << CURRENT_TIME << INFO_MESSAGE << "server listening on port '" << port << "'" << std::endl;
					}else{
						std::cout << CURRENT_TIME << ERROR_MESSAGE << "couldn't start new thread for server" << std::endl;
					}
				}catch (const std::exception& exception) {
					std::cout << CURRENT_TIME << ERROR_MESSAGE << exception.what() << std::endl;
				}
			}else{
				std::cout << CURRENT_TIME << ERROR_MESSAGE << "you already connected to a host. so you can't start your host server" << std::endl;
			}
		}else{
			std::cout << CURRENT_TIME << ERROR_MESSAGE << "server is already running" << std::endl;
		}
	}

	SetConsoleColorMode(ConsoleMode::Input);
}

Command StopServer(Arguments) {
	SetConsoleColorMode(ConsoleMode::Output);

	if (g_HostManager->IsRunning()) {
		try{
			g_HostManager->Stop();
			
			if (g_ServerThread) {
				TerminateThread(g_ServerThread->native_handle(), 0);
				g_ServerThread->detach();

				delete g_ServerThread;
				g_ServerThread = NULL;
			}else{
				std::cout << CURRENT_TIME << WARNING_MESSAGE << "couldn't release server thread" << std::endl;
			}

			SetConsoleColorMode(ConsoleMode::Output);
			std::cout << CURRENT_TIME << INFO_MESSAGE << "server stopped" << std::endl;
		}catch (const std::exception& exception) {
			std::cout << CURRENT_TIME << ERROR_MESSAGE << exception.what() << std::endl;
		}
	}else{
		std::cout << CURRENT_TIME << ERROR_MESSAGE << "server is not running" << std::endl;
	}

	SetConsoleColorMode(ConsoleMode::Input);
}

Command ServerStatus(Arguments) {
	SetConsoleColorMode(ConsoleMode::Output);

	if (g_HostManager->IsRunning()) {
		std::cout << CURRENT_TIME << INFO_MESSAGE << "server is running on port '" << g_HostManager->GetServerPort() << "'" << std::endl;
		
		if (g_HostManager->IsConnected()) {
			std::cout << CURRENT_TIME << INFO_MESSAGE << "client connected!" << std::endl;
		}else{
			std::cout << CURRENT_TIME << INFO_MESSAGE << "couldn't find any connections" << std::endl;
		}
	}else{
		std::cout << CURRENT_TIME << ERROR_MESSAGE << "server is not running" << std::endl;
	}

	SetConsoleColorMode(ConsoleMode::Input);
}

Command ServerPort(Arguments) {
	SetConsoleColorMode(ConsoleMode::Output);

	if (g_HostManager->IsRunning()) {
		std::cout << CURRENT_TIME << INFO_MESSAGE << "server is running on port '" << g_HostManager->GetServerPort() << std::endl;
	}else{
		std::cout << CURRENT_TIME << ERROR_MESSAGE << "server is not running" << std::endl;
	}

	SetConsoleColorMode(ConsoleMode::Input);
}

Command Connect(Arguments) {
	SetConsoleColorMode(ConsoleMode::Output);

	std::string host = get(arguments, 0);
	if (host.empty()) {
		std::cout << CURRENT_TIME << ERROR_MESSAGE << "you must pass server ip address as first argument of this command" << std::endl;
	}else{
		std::string portString = get(arguments, 1);
		if (portString.empty()) {
			std::cout << CURRENT_TIME << ERROR_MESSAGE << "you must pass port number as second argument of this command" << std::endl;
		}else{
			int port = std::stoi(portString);
			
			if (g_HostManager->IsRunning()) {
				std::cout << CURRENT_TIME << ERROR_MESSAGE << "you already started a host. so you can't connect to any other hosts" << std::endl;
			}else{
				if (g_ClientConnection->IsConnected()) {
					std::cout << CURRENT_TIME << ERROR_MESSAGE << "you already connected to host '" << g_ClientConnection->GetServerHost() << ":" << g_ClientConnection->GetServerPort() << "'" << std::endl;
				}else{
					try {
						g_ClientConnection->SetServerHost(host);
						g_ClientConnection->SetServerPort(port);

						g_ClientConnection->Connect();

						if (g_ClientThread) {
							TerminateThread(g_ClientThread->native_handle(), 0);
							g_ClientThread->detach();

							delete g_ClientThread;
							g_ClientThread = NULL;
						}

						g_ClientThread = new std::thread(ClientConnectionProcess);

						std::cout << CURRENT_TIME << INFO_MESSAGE << "client connected to host[" << host << ":" << port << "]. use 'say' to send messages" << std::endl;
					}catch (const std::exception& exception) {
						std::cout << CURRENT_TIME << ERROR_MESSAGE << exception.what() << std::endl;
					}
				}
			}
		}
	}

	SetConsoleColorMode(ConsoleMode::Input);
}

Command Disconnect(Arguments) {
	SetConsoleColorMode(ConsoleMode::Output);

	if (g_ClientConnection->IsConnected()) {
		try {
			g_ClientConnection->Disconnect();

			if (g_ClientThread) {
				TerminateThread(g_ClientThread->native_handle(), 0);
				g_ClientThread->detach();

				delete g_ClientThread;
				g_ClientThread = NULL;
			}

			SetConsoleColorMode(ConsoleMode::Output);
			std::cout << CURRENT_TIME << INFO_MESSAGE << "disconnected from host" << std::endl;
		}catch (const std::exception& exception) {
			std::cout << CURRENT_TIME << ERROR_MESSAGE << exception.what() << std::endl;
		}
	}else{
		std::cout << CURRENT_TIME << ERROR_MESSAGE << "you didn't connect to any hosts yet" << std::endl;
	}

	SetConsoleColorMode(ConsoleMode::Input);
}

Command ConnectionStatus(Arguments) {
	SetConsoleColorMode(ConsoleMode::Output);

	if (g_ClientConnection->IsConnected()) {
		std::cout << CURRENT_TIME << INFO_MESSAGE << "connected to host[" << g_ClientConnection->GetServerHost() << ":" << g_ClientConnection->GetServerPort() << "]" << std::endl;
	}else{
		std::cout << CURRENT_TIME << INFO_MESSAGE << "didn't connect to any hosts yet" << std::endl;
	}

	SetConsoleColorMode(ConsoleMode::Input);
}

Command ConnectionIP(Arguments) {
	SetConsoleColorMode(ConsoleMode::Output);

	if (g_ClientConnection->IsConnected()) {
		std::cout << CURRENT_TIME << INFO_MESSAGE << "connected to host with ip '" << g_ClientConnection->GetServerHost() << "'" << std::endl;
	}else{
		std::cout << CURRENT_TIME << ERROR_MESSAGE << "you didn't connect to any hosts yet" << std::endl;
	}

	SetConsoleColorMode(ConsoleMode::Input);
}

Command ConnectionPort(Arguments) {
	SetConsoleColorMode(ConsoleMode::Output);

	if (g_ClientConnection->IsConnected()) {
		std::cout << CURRENT_TIME << INFO_MESSAGE << "connected to host with port '" << g_ClientConnection->GetServerPort() << "'" << std::endl;
	}else{
		std::cout << CURRENT_TIME << ERROR_MESSAGE << "you didn't connect to any hosts yet" << std::endl;
	}

	SetConsoleColorMode(ConsoleMode::Input);
}

Command Say(Arguments) {
	SetConsoleColorMode(ConsoleMode::Output);

	std::string message;

	for (std::string& chunk : arguments) {
		message += chunk;
		message += " ";
	}

	if (!message.empty()) message.pop_back();

	if (message.empty()) {
		std::cout << CURRENT_TIME << ERROR_MESSAGE << "message can't be empty" << std::endl;
	}else{
		SocketData data;
		data.type = SHARED_EVENTS_TYPE_ONSAY;
		data.content = message;

		std::string username;
		GetWindowsUsername(&username);

		data.entries.push_back({"username", username});

		if (g_HostManager->IsRunning()) {
			if (g_HostManager->IsConnected()) {
				try {
					g_HostManager->TriggerClientEvent(data);

					std::cout << CURRENT_TIME << INFO_MESSAGE << "message sent" << std::endl;
				}catch (const std::exception& exception) {
					std::cout << CURRENT_TIME << ERROR_MESSAGE << exception.what() << std::endl;
				}
			}else{
				std::cout << CURRENT_TIME << ERROR_MESSAGE << "you didn't connect to any clients yet" << std::endl;
			}
		}else{
			if (g_ClientConnection->IsConnected()) {
				try {
					g_ClientConnection->TriggerServerEvent(data);

					std::cout << CURRENT_TIME << INFO_MESSAGE << "message sent" << std::endl;
				}catch (const std::exception& exception) {
					std::cout << CURRENT_TIME << ERROR_MESSAGE << exception.what();
				}
			}else{
				std::cout << CURRENT_TIME << ERROR_MESSAGE << "you didn't connect to any hosts yet" << std::endl;
			}
		}
	}

	SetConsoleColorMode(ConsoleMode::Input);
}

Command SendFile(Arguments) {

}

Command SendDir(Arguments) {

}

Command CancelTransfer(Arguments) {

}

Command IsBusy(Arguments) {

}

std::string get(Arguments, const int& index) {
	if (index < arguments.size() && index > -1) {
		return arguments[index];
	}
	return std::string();
}

bool isint(const std::string& str) {
	if (str.empty()) {
		return false;
	}
	bool result = true;
	int length = str.length();
	for (int i = 0; i < length; i++) {
		if (!std::isdigit(str[i])) {
			result = false;
			break;
		}
	}
	return result;
}

#undef Command
#undef Arguments