#pragma once

#ifndef CONSOLE_COMMAND_HEADER
#define CONSOLE_COMMAND_HEADER

#include "CommandControl.h"

#define CONSOLE_COMMAND_HELP "help"
#define CONSOLE_COMMAND_EXIT "exit"
#define CONSOLE_COMNAND_WORKINGDIR "workingdir"

#define CONSOLE_COMMAND_ADAPTERS "adapters"
#define CONSOLE_COMMAND_HASWIFIADAPTER "haswifiadapter"
#define CONSOLE_COMMAND_WIFIADDRESS "wifiaddress"

class ConsoleCommands {
public:
	static void Help(CommandArguments&);
	static void Exit(CommandArguments&);
	static void WorkingDir(CommandArguments&);

	static void Adapters(CommandArguments&);
	static void HasWifiAdapter(CommandArguments&);
	static void WifiAddress(CommandArguments&);
};

#endif