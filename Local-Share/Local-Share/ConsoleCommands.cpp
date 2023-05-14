#include "ConsoleCommands.h"

#include <iostream>
#include <string.h>

#include "Utility.h"

extern bool ApplicationRunning;
extern CommandControl* g_CommandManager;

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