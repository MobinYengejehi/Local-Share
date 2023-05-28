#include <iostream>
#include <thread>
#include <string>
#include <filesystem>

#include "Utility.h"
#include "ConsoleCommands.h"
#include "Local-Share.h"
#include "Config.h"
#include "HostManager.h"
#include "SharedEvents.h"

#define C ConsoleCommands::
#define SH SharedEvents::

void OnClientConnect(SocketData* data) {
	SetConsoleColorMode(ConsoleMode::Output);
	std::cout << CURRENT_TIME << INFO_MESSAGE << "client connected to host" << std::endl;
	SetConsoleColorMode(ConsoleMode::Input);
}

void OnClientDisconnect(SocketData* data) {
	FileProcessingCleanup();

	SetConsoleColorMode(ConsoleMode::Output);
	std::cout << CURRENT_TIME << INFO_MESSAGE << "client disconnected from host";
	if (data->HasEntry("reason")) {
		std::cout << ". [reason : " << data->GetEntryValue("reason") << "]";
	}
	std::cout << std::endl;
	SetConsoleColorMode(ConsoleMode::Input);
}

void OnDisconnectFromHost(SocketData* data) {
	FileProcessingCleanup();

	SetConsoleColorMode(ConsoleMode::Output);
	std::cout << CURRENT_TIME << WARNING_MESSAGE << "you disconnected from host[" << g_ClientConnection->GetServerHost() << ":" << g_ClientConnection->GetServerPort() << "]" << std::endl;
	SetConsoleColorMode(ConsoleMode::Input);
}

void RegisterConsoleCommandHandlers() {
	if (!g_CommandManager) {
		return;
	}

	g_CommandManager->AddCommand(CONSOLE_COMMAND_HELP, C Help);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_EXIT, C Exit);
	g_CommandManager->AddCommand(CONSOLE_COMNAND_WORKINGDIR, C WorkingDir);
	
	g_CommandManager->AddCommand(CONSOLE_COMMAND_ADAPTERS, C Adapters);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_HASWIFIADAPTER, C HasWifiAdapter);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_WIFIADDRESS, C WifiAddress);

	g_CommandManager->AddCommand(CONSOLE_COMMAND_STARTSERVER, C StartServer);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_STOPSERVER, C StopServer);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_SERVERSTATUS, C ServerStatus);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_SERVERPORT, C ServerPort);

	g_CommandManager->AddCommand(CONSOLE_COMMAND_CONNECT, C Connect);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_DISCONNECT, C Disconnect);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_CONNECTIONSTATUS, C ConnectionStatus);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_CONNECTIONIP, C ConnectionIP);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_CONNECTIONPORT, C ConnectionPort);

	g_CommandManager->AddCommand(CONSOLE_COMMAND_SAY, C Say);

	g_CommandManager->AddCommand(CONSOLE_COMMAND_SENDFILE, C SendFile);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_SENDDIR, C SendDir);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_CANCELTRANSFER, C CancelTransfer);

	g_CommandManager->AddCommand(CONSOLE_COMMAND_ISBUSY, C IsBusy);

	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_HELP, COMMAND_DESCRIPTION_HELP);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_EXIT, COMMAND_DESCRIPTION_EXIT);
	g_CommandManager->AddCommandDescription(CONSOLE_COMNAND_WORKINGDIR, COMMAND_DESCRIPTION_WORKINGDIR);

	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_ADAPTERS, COMMAND_DESCRIPTION_ADAPTERS);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_HASWIFIADAPTER, COMMAND_DESCRIPTION_HASWIFIADAPTER);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_WIFIADDRESS, COMMAND_DESCRIPTION_WIFIADDRESS);

	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_STARTSERVER, COMMAND_DESCRIPTION_STARTSERVER);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_STOPSERVER, COMMAND_DESCRIPTION_STOPSERVER);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_SERVERSTATUS, COMMAND_DESCRIPTION_SERVERSTATUS);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_SERVERPORT, COMMAND_DESCRIPTION_SERVERPORT);

	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_CONNECT, COMMAND_DESCRIPTION_CONNECT);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_DISCONNECT, COMMAND_DESCRIPTION_DISCONNECT);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_CONNECTIONSTATUS, COMMAND_DESCRIPTION_CONNECTIONSTATUS);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_CONNECTIONIP, COMMAND_DESCRIPTION_CONNECTIONIP);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_CONNECTIONPORT, COMMAND_DESCRIPTION_CONNECTIONPORT);

	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_SAY, COMMAND_DESCRIPTION_SAY);
	
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_SENDFILE, COMMAND_DESCRIPTION_SENDFILE);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_SENDDIR, COMMAND_DESCRIPTION_SENDDIR);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_CANCELTRANSFER, COMMAND_DESCRIPTION_CANCELTRANSFER);

	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_ISBUSY, COMMAND_DESCRIPTION_ISBUSY);
}

void DataShareServerProcessFace() {
	if (!g_HostManager) {
		return;
	}

	while (ApplicationRunning) {
		g_HostManager->ProcessConnection();
	}
}

void ClientConnectionProcessFace() {
	if (!g_ClientConnection) {
		return;
	}

	while (ApplicationRunning) {
		g_ClientConnection->ProcessConnection();
	}
}

void InputProcess() {
	g_CommandManager = new CommandControl();
	if (g_CommandManager) {
		RegisterConsoleCommandHandlers();

		while (ApplicationRunning) {
			std::getline(std::cin, g_CommandManager->GetInput());
			g_CommandManager->BuildCommand();

			g_CommandManager->ExecuteCommand(g_CommandManager->GetCommand());
			g_CommandManager->ClearArguments();
		}
	}else{
		SetConsoleColorMode(ConsoleMode::Output);
		std::cout << CURRENT_TIME << ERROR_MESSAGE << "couldn't allocate memory for command controller" << std::endl;
		SetConsoleColorMode(ConsoleMode::Input);
	}
}

void ShowStartupInfo() {
	std::string Undefined = "Undefined";

	std::string workingDirectory;
	GetApplicationDirectory(&workingDirectory);

	NetAdapter wifi;
	GetWifiAdapter(&wifi);

	#define CheckEmpty(str) (str.empty() ? Undefined : str)

	SetConsoleColorMode(ConsoleMode::Output);
	std::cout << "--------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << "----" << std::endl;
	std::cout << "---- Server Name       : " << APPLICATION_NAME << std::endl;
	std::cout << "---- Version           : " << GetFilteredVersion(APPLICATION_VERSION) << std::endl;
	std::cout << "---- Working Directory : " << (workingDirectory.empty() ? "." : workingDirectory) << std::endl;
	std::cout << "----" << std::endl;
	std::cout << "---- Has Wifi Adapter : " << ((HasWifiAdapter() && IsValidNetAdapter(wifi)) ? "Yes" : "No") << std::endl;
	std::cout << "---- Wifi Ip Address  : " << CheckEmpty(wifi.ip) << std::endl;
	std::cout << "----" << std::endl;
	std::cout << "--------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "use `help` to see commands" << std::endl;
	SetConsoleColorMode(ConsoleMode::Input);

	#undef CheckEmpty

	SetConsoleTitleA(APPLICATION_NAME);
}

int main() {
	FileBuffer = malloc(FILE_SENDING_BUFFER_SIZE);

	g_HostManager = new HostManager();
	g_HostManager->SetServerHost(ANYMACHINE);
	g_HostManager->SetServerPort(DEFAULT_PORT);

	EventManager& hostEvents = g_HostManager->GetEventManager();

	hostEvents.AddHandler(SOCKET_CLIENT_CONNECT_MESSAGE_TYPE, OnClientConnect);
	hostEvents.AddHandler(SOCKET_CLIENT_DISCONNECT_MESSAGE_TYPE, OnClientDisconnect);

	hostEvents.AddHandler(SHARED_EVENTS_TYPE_ONSAY, SH OnSay);

	hostEvents.AddHandler(SHARED_EVENTS_TYPE_NEWFILE, SH NewFile);
	hostEvents.AddHandler(SHARED_EVENTS_TYPE_REQUESTFILECHUNK, SH RequestFileChunk);
	hostEvents.AddHandler(SHARED_EVENTS_TYPE_RECIEVEFILECHUNK, SH RecieveFileChunk);
	
	hostEvents.AddHandler(SHARED_EVENTS_TYPE_INTERRUPTED, SH Interrupted);

	hostEvents.AddHandler(SHARED_EVENTS_TYPE_END, SH End);

	g_ClientConnection = new ClientConnection();
	g_ClientConnection->SetServerHost(LOCALHOST);
	g_ClientConnection->SetServerPort(DEFAULT_PORT);

	EventManager& clientEvents = g_ClientConnection->GetEventManager();

	clientEvents.AddHandler(CLIENT_DISCONNECT_MESSAGE_TYPE, OnDisconnectFromHost);

	clientEvents.AddHandler(SHARED_EVENTS_TYPE_ONSAY, SH OnSay);

	clientEvents.AddHandler(SHARED_EVENTS_TYPE_NEWFILE, SH NewFile);
	clientEvents.AddHandler(SHARED_EVENTS_TYPE_REQUESTFILECHUNK, SH RequestFileChunk);
	clientEvents.AddHandler(SHARED_EVENTS_TYPE_RECIEVEFILECHUNK, SH RecieveFileChunk);

	clientEvents.AddHandler(SHARED_EVENTS_TYPE_INTERRUPTED, SH Interrupted);

	clientEvents.AddHandler(SHARED_EVENTS_TYPE_END, SH End);

	DataShareServerProcess = DataShareServerProcessFace;
	ClientConnectionProcess = ClientConnectionProcessFace;

	ShowStartupInfo();

	std::thread InputThread(InputProcess);

	InputThread.join();

	if (g_ServerThread) {
		TerminateThread(g_ServerThread->native_handle(), 1);
		g_ServerThread->detach();
		g_ServerThread = NULL;
	}
	if (g_ClientThread) {
		TerminateThread(g_ClientThread->native_handle(), 1);
		g_ClientThread->detach();
		g_ClientThread = NULL;
	}

	HostManager::CleanUp();

	return 0;
}