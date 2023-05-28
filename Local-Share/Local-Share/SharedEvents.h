#pragma once

#ifndef SHARED_EVENTS_HEADER
#define SHARED_EVENTS_HEADER

#include <string>

#include "SocketData.h"
#include "Utility.h"

#define SHARED_EVENTS_TYPE_ONSAY "say"

#define SHARED_EVENTS_TYPE_NEWFILE "nf"
#define SHARED_EVENTS_TYPE_REQUESTFILECHUNK "rfc"
#define SHARED_EVENTS_TYPE_RECIEVEFILECHUNK "rcfc"

#define SHARED_EVENTS_TYPE_INTERRUPTED "intr"

#define SHARED_EVENTS_TYPE_END "end"

class SharedEvents {
public:
	static void OnSay(SocketData*);

	static void NewFile(SocketData*);
	static void RequestFileChunk(SocketData*);
	static void RecieveFileChunk(SocketData*);

	static void Interrupted(SocketData*);

	static void End(SocketData*);
};

#endif