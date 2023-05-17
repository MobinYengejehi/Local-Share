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

#define CONSOLE_COMMAND_STARTSERVER "startserver"
#define CONSOLE_COMMAND_STOPSERVER "stopserver"
#define CONSOLE_COMMAND_SERVERSTATUS "serverstatus"
#define CONSOLE_COMMAND_SERVERPORT "serverport"

#define CONSOLE_COMMAND_CONNECT "connect"
#define CONSOLE_COMMAND_DISCONNECT "disconnect"
#define CONSOLE_COMMAND_CONNECTIONSTATUS "connectionstatus"
#define CONSOLE_COMMAND_CONNECTIONIP "connectionip"
#define CONSOLE_COMMAND_CONNECTIONPORT "connectionport"

#define CONSOLE_COMMAND_SAY "say"

#define CONSOLE_COMMAND_SENDFILE "sendfile"
#define CONSOLE_COMMAND_SENDDIR "senddir"
#define CONSOLE_COMMAND_CANCELTRANSFER "canceltransfer"

#define CONSOLE_COMMAND_ISBUSY "isbusy"

class ConsoleCommands {
public:
	static void Help(CommandArguments&);
	static void Exit(CommandArguments&);
	static void WorkingDir(CommandArguments&);

	static void Adapters(CommandArguments&);
	static void HasWifiAdapter(CommandArguments&);
	static void WifiAddress(CommandArguments&);

	static void StartServer(CommandArguments&);
	static void StopServer(CommandArguments&);
	static void ServerStatus(CommandArguments&);
	static void ServerPort(CommandArguments&);

	static void Connect(CommandArguments&);
	static void Disconnect(CommandArguments&);
	static void ConnectionStatus(CommandArguments&);
	static void ConnectionIP(CommandArguments&);
	static void ConnectionPort(CommandArguments&);

	static void Say(CommandArguments&);

	static void SendFile(CommandArguments&);
	static void SendDir(CommandArguments&);
	static void CancelTransfer(CommandArguments&);

	static void IsBusy(CommandArguments&);
};

#endif