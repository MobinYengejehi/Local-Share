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

	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_HELP, COMMAND_DESCRIPTION_HELP);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_EXIT, COMMAND_DESCRIPTION_EXIT);
	g_CommandManager->AddCommandDescription(CONSOLE_COMNAND_WORKINGDIR, COMMAND_DESCRIPTION_WORKINGDIR);

	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_ADAPTERS, COMMAND_DESCRIPTION_ADAPTERS);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_HASWIFIADAPTER, COMMAND_DESCRIPTION_HASWIFIADAPTER);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_WIFIADDRESS, COMMAND_DESCRIPTION_WIFIADDRESS);
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
	ShowStartupInfo();

	std::thread InputThread(InputProcess);

	InputThread.join();

	return 0;
}

/*void ApplicationProcess() {
	unsigned long long i = 0;
	while (true) {
		if (i%10000000 == 0) std::cout << "i is : " << i << std::endl;
		i++;
	}
}

void InputProcess(bool* terminated, std::thread* thread) {
	while (true) {
		std::string input;
		std::getline(std::cin, input);
		if (!*terminated) {
			std::cout << "terminating thread ..." << std::endl;
			TerminateThread(thread->native_handle(), 1);
			thread->detach();
			*terminated = true;
			std::cout << "thread teminated" << std::endl;
		}else{
			if (input == "exit") {
				break;
			}
		}
	}
}

int main() {
	std::cout << "this is test honey!" << std::endl;

	bool threadTerminated = false;
    std::thread Process(ApplicationProcess);
	std::thread Input(InputProcess, &threadTerminated, &Process);

	Input.join();

	return 0;
}*/