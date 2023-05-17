#pragma once

#ifndef SHARED_EVENTS_HEADER
#define SHARED_EVENTS_HEADER

#include <string>

#include "SocketData.h"
#include "Utility.h"

#define SHARED_EVENTS_TYPE_ONSAY "say"

#define SHARED_EVENTS_TYPE_ONCREATEFILE "cf"
#define SHARED_EVENTS_TYPE_ONCREATEDIRECTORY "cd"

#define SHARED_EVENTS_TYPE_ONRECIEVEFILEDATA "rcv"
#define SHARED_EVENTS_TYPE_ONGOTCHUNK "got"

#define SHARED_EVENTS_TYPE_ONREQUESTNEXTFILE "nt"

class SharedEvents {
public:
	static void OnSay(SocketData*);

	static void OnCreateFile(SocketData*);
	static void OnCreateDirectory(SocketData*);

	static void OnRecieveFileData(SocketData*);
	static void OnGotChunk(SocketData*);

	static void OnRequestNextFile(SocketData*);
};

#endif