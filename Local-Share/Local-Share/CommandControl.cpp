#include "CommandControl.h"

#define COMMAND_ARGUMENT_SEPERATOR " "

void CommandControl::BuildCommand() {
	if (Input.empty()) {
		return;
	}
	CommandArguments args;
	SplitString(Input, COMMAND_ARGUMENT_SEPERATOR, &args);
	Command = args[0];
	size_t length = args.size();
	ClearArguments();
	if (length > 1) {
		for (size_t i = 1; i < length; i++) {
			Arguments.push_back(args[i]);
		}
	}
}

void CommandControl::AddCommand(const std::string& command, CommandHandler handler) {
	if (command.empty()) {
		return;
	}
	if (handler) {
		std::vector<CommandHandler>& handlers = CommandHandlers[command];
		if (ExistsCommand(command, handler)) {
			return;
		}
		handlers.push_back(handler);
	}
}

void CommandControl::RemoveCommand(const std::string& command, CommandHandler handler) {
	if (ExistsCommand(command, handler)) {
		std::vector<CommandHandler>& handlers = CommandHandlers[command];
		if (handlers.empty()) {
			return;
		}
		auto end = std::remove_if(handlers.begin(), handlers.end(), [&handler](CommandHandler hndl) {
			if (hndl == handler) {
				return true;
			}
			return false;
			});
		handlers.resize(std::distance(handlers.begin(), end));
	}
}

bool CommandControl::ExecuteCommand(const std::string& command) {
	if (!command.empty()) {
		std::vector<CommandHandler>& handlers = CommandHandlers[command];
		if (!handlers.empty()) {
			size_t length = handlers.size();
			for (size_t i = 0; i < length; i++) {
				CommandHandler handler = handlers[i];
				if (handler) {
					handler(Arguments);
				}
			}
			return true;
		}
	}
	return false;
}

void CommandControl::AddCommandDescription(const std::string& command, const std::string& description) {
	if (command.empty()) {
		return;
	}
	if (description.empty()) {
		return;
	}
	if (ExistsCommandDescription(command)) {
		return;
	}
	CommandDescriptions[command] = description;
}

void CommandControl::RemoveCommandDescription(const std::string& command) {
	if (command.empty()) {
		return;
	}
	if (!ExistsCommandDescription(command)) {
		return;
	}
	CommandDescriptions.erase(command);
}

bool CommandControl::ExistsCommand(const std::string& command, CommandHandler handler) {
	if (!command.empty() && handler) {
		std::vector<CommandHandler>& handlers = CommandHandlers[command];
		if (!handlers.empty()) {
			for (CommandHandler handl : handlers) {
				if (handler == handl) {
					return true;
				}
			}
		}
	}
	return false;
}

bool CommandControl::ExistsCommandDescription(const std::string& command) {
	if (!command.empty()) {
		std::string& description = CommandDescriptions[command];
		if (!description.empty()) {
			return true;
		}
	}
	return false;
}

std::string& CommandControl::GetInput() {
	return Input;
}

std::string& CommandControl::GetCommand() {
	return Command;
}

std::string CommandControl::GetCommandDescription(const std::string& command) {
	if (ExistsCommandDescription(command)) {
		return CommandDescriptions[command];
	}
	return std::string();
}

CommandArguments& CommandControl::GetArguments() {
	return Arguments;
}

CommandHandlerMap& CommandControl::GetCommandHandlers() {
	return CommandHandlers;
}

StringMap& CommandControl::GetCommandDescriptions() {
	return CommandDescriptions;
}

void CommandControl::ClearArguments() {
	Arguments = CommandArguments();
}