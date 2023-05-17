#pragma once

#ifndef LOCAL_SHARE_HEADER
#define LOCAL_SHARE_HEADER

#include <thread>

#include "CommandControl.h"
#include "HostManager.h"
#include "ClientConnection.h"

typedef void (*VoidFunction)();

bool ApplicationRunning = true;
 
CommandControl* g_CommandManager = nullptr;

HostManager*      g_HostManager = nullptr;
ClientConnection* g_ClientConnection = nullptr;

std::thread* g_ServerThread = nullptr;
std::thread* g_ClientThread = nullptr;

VoidFunction DataShareServerProcess;
VoidFunction ClientConnectionProcess;

#endif