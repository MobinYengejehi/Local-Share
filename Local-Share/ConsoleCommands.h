#pragma once

#ifndef CONSOLE_COMMAND_HEADER
#define CONSOLE_COMMAND_HEADER

#include "CommandControl.h"

#define CONSOLE_COMMAND_HELP "help"
#define CONSOLE_COMMAND_EXIT "exit"
#define CONSOLE_COMNAND_WORKINGDIR "workingdir"

class ConsoleCommands {
public:
	static void Help(CommandArguments&);
	static void Exit(CommandArguments&);
	static void WorkingDir(CommandArguments&);
};

#endif