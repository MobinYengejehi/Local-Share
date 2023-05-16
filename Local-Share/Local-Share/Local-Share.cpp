#include <iostream>
#include <thread>
#include <string>

#include "Utility.h"
#include "ConsoleCommands.h"
#include "Local-Share.h"
#include "Config.h"
#include "HostManager.h"

#define C ConsoleCommands::

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

	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_SAY, COMMAND_DESCRIPTION_SAY);
	
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_SENDFILE, COMMAND_DESCRIPTION_SENDFILE);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_SENDDIR, COMMAND_DESCRIPTION_SENDDIR);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_CANCELTRANSFER, COMMAND_DESCRIPTION_CANCELTRANSFER);

	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_ISBUSY, COMMAND_DESCRIPTION_ISBUSY);
}

void DataShareServerProcess() {
	if (!g_HostManager) {
		return;
	}

	while (ApplicationRunning) {
		g_HostManager->ProcessConnection();
	}
}

void ClientConnectionProcess() {
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
	g_HostManager = new HostManager();
	g_HostManager->SetServerHost(ANYMACHINE);
	g_HostManager->SetServerPort(DEFAULT_PORT);

	g_ClientConnection = new ClientConnection();
	g_ClientConnection->SetServerHost(LOCALHOST);
	g_ClientConnection->SetServerPort(DEFAULT_PORT);

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

	return 0;
}