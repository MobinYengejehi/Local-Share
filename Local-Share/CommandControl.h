#pragma once

#ifndef COMMAND_CONTROL_HEADER
#define COMMAND_CONTROL_HEADER

#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "Utility.h"

typedef StringList CommandArguments;
typedef void (*CommandHandler)(CommandArguments&);
typedef std::map<std::string, std::vector<CommandHandler>> CommandHandlerMap;

class CommandControl {
public:
	CommandControl() = default;
	~CommandControl() = default;

	void BuildCommand();
	void AddCommand(const std::string&, CommandHandler);
	void RemoveCommand(const std::string&, CommandHandler);
	bool ExecuteCommand(const std::string&);

	void AddCommandDescription(const std::string&, const std::string&);
	void RemoveCommandDescription(const std::string&);

	bool ExistsCommand(const std::string&, CommandHandler);
	bool ExistsCommandDescription(const std::string&);

	std::string& GetInput();
	std::string& GetCommand();
	std::string        GetCommandDescription(const std::string&);
	CommandArguments& GetArguments();
	CommandHandlerMap& GetCommandHandlers();
	StringMap& GetCommandDescriptions();

	void ClearArguments();
private:
	std::string       Input;
	std::string       Command;
	CommandArguments  Arguments;
	CommandHandlerMap CommandHandlers;
	StringMap         CommandDescriptions;
};

#endif