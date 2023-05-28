#pragma once

#ifndef LOCAL_SHARE_HEADER
#define LOCAL_SHARE_HEADER

#include <filesystem>
#include <thread>

#include "CommandControl.h"
#include "HostManager.h"
#include "ClientConnection.h"
#include "FolderQueue.h"

typedef void (*VoidFunction)();

bool ApplicationRunning = true;
 
CommandControl* g_CommandManager = nullptr;

HostManager*      g_HostManager = nullptr;
ClientConnection* g_ClientConnection = nullptr;

std::thread* g_ServerThread = nullptr;
std::thread* g_ClientThread = nullptr;

VoidFunction DataShareServerProcess;
VoidFunction ClientConnectionProcess;

FILE*  WorkingFile = NULL;
void*  FileBuffer = NULL;
size_t FileBytesRead = 0;
size_t FileSize = 0;
bool   IsProcessingFile = false;

FolderQueue* FolderInterface = NULL;

#endif