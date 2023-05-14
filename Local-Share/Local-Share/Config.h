#pragma once

#ifndef CONFIG_HEADER
#define CONFIG_HEADER

#define LOCALHOST "127.0.0.1"
#define ANYMACHINE "0.0.0.0"
#define DEFAULT_PORT 80

#define APPLICATION_NAME "Local Share"
#define APPLICATION_VERSION 1.0f

#define COMMAND_DESCRIPTION_HELP "[command name (optional)] = this command shows a list of commands or information about a specific command"
#define COMMAND_DESCRIPTION_EXIT "if you execute this command it will stop all servers and will dellocate the memory and after that will close the server console"
#define COMMAND_DESCRIPTION_WORKINGDIR "this command shows the directory which console is working on"

#define COMMAND_DESCRIPTION_ADAPTERS "this command shows a list of system adapters"
#define COMMAND_DESCRIPTION_HASWIFIADAPTER "this command shows if system has wifi adapter of not"
#define COMMAND_DESCRIPTION_WIFIADDRESS "this command shows wifi ip address"

#define COMMAND_DESCRIPTION_STARTSERVER "[port] = this command starts data share server"
#define COMMAND_DESCRIPTION_STOPSERVER "this command stops data share server"
#define COMMAND_DESCRIPTION_SERVERSTATUS "this command shows if data share server is running or not"
#define COMMAND_DESCRIPTION_SERVERPORT "this command shows the port of data share server"

#define COMMAND_DESCRIPTION_CONNECT "[ip address] [port] = this command connects to a data share server if not already connected"
#define COMMAND_DESCRIPTION_DISCONNECT "this command disconnects from data share server"
#define COMMAND_DESCRIPTION_CONNECTIONSTATUS "this command shows if system connected to a data share server or not"
#define COMMAND_DESCRIPTION_CONNECTIONIP "this command shows the connection ip"
#define COMMAND_DESCRIPTION_CONNECTIONPORT "this command shows the connection port"

#endif