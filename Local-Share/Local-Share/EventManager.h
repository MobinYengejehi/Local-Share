#pragma once

#ifndef EVENT_MANAGER_HEADER
#define EVENT_MANAGER_HEADER

#include <string>
#include <vector>
#include <map>

#include "SocketData.h"

typedef void (*EventHandler)(SocketData*);
typedef std::vector<EventHandler> EventHandlerList;
typedef std::map<std::string, EventHandlerList> EventHandlerMap;

class EventManager {
public:
	EventManager();
	~EventManager() = default;

	EventHandlerList& GetHandlerList();
	EventHandlerMap&  GetSpecialHandlerMap();
	EventHandlerList& GetSpecialHandlers(const std::string&);

	bool ExistsHandler(const std::string&, EventHandler);
	bool ExistsHandler(EventHandler);
	
	void AddHandler(const std::string&, EventHandler);
	void AddHandler(EventHandler);
	
	void RemoveHandler(const std::string&, EventHandler);
	void RemoveHandler(EventHandler);

	void Trigger(SocketData&);

	void Cancel();

	bool IsCanceled();
private:
	bool canceled;

	EventHandlerList handlers;
	EventHandlerMap  specialHandlers;
};

#endif