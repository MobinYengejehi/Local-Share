#pragma once

#ifndef LOCAL_SHARE_HEADER
#define LOCAL_SHARE_HEADER

#include "CommandControl.h"
#include "HostManager.h"

bool ApplicationRunning = true;
 
CommandControl* g_CommandManager = nullptr;
HostManager*    g_HostManager = nullptr;

#endif