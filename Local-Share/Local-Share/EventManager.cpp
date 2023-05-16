#include "EventManager.h"

EventManager::EventManager() {
	canceled = false;
}

EventHandlerList& EventManager::GetHandlerList() {
	return handlers;
}

EventHandlerMap& EventManager::GetSpecialHandlerMap() {
	return specialHandlers;
}

EventHandlerList& EventManager::GetSpecialHandlers(const std::string& type) {
	EventHandlerList& list = specialHandlers[type];
	return list;
}

bool EventManager::ExistsHandler(const std::string& type, EventHandler handler) {
	if (type.empty()) {
		return false;
	}
	if (!handler) {
		return false;
	}

	EventHandlerList& list = specialHandlers[type];
	if (list.empty()) {
		return false;
	}

	for (EventHandler hndlr : list) {
		if (hndlr == handler) {
			return true;
		}
	}

	return false;
}

bool EventManager::ExistsHandler(EventHandler handler) {
	if (!handler) {
		return false;
	}

	if (handlers.empty()) {
		return false;
	}

	for (EventHandler hndlr : handlers) {
		if (hndlr == handler) {
			return true;
		}
	}
	return false;
}

void EventManager::AddHandler(const std::string& type, EventHandler handler) {
	if (type.empty()) {
		return;
	}
	if (!handler) {
		return;
	}

	if (ExistsHandler(type, handler)) {
		return;
	}

	EventHandlerList& list = specialHandlers[type];
	list.push_back(handler);
}

void EventManager::AddHandler(EventHandler handler) {
	if (!handler) {
		return;
	}

	if (ExistsHandler(handler)) {
		return;
	}

	handlers.push_back(handler);
}

void EventManager::RemoveHandler(const std::string& type, EventHandler handler) {
	if (type.empty()) {
		return;
	}
	if (!handler) {
		return;
	}

	if (!ExistsHandler(type, handler)) {
		return;
	}

	EventHandlerList& list = specialHandlers[type];
	if (list.empty()) {
		return;
	}

	size_t length = list.size();

	for (size_t i = 0; i < length; i++) {
		if (list[i] == handler) {
			list.erase(list.begin() + i);
			break;
		}
	}
}

void EventManager::RemoveHandler(EventHandler handler) {
	if (!handler) {
		return;
	}

	if (handlers.empty()) {
		return;
	}

	if (!ExistsHandler(handler)) {
		return;
	}

	size_t length = handlers.size();

	for (size_t i = 0; i < length; i++) {
		if (handlers[i] == handler) {
			handlers.erase(handlers.begin() + i);;
			break;
		}
	}
}

void EventManager::Trigger(SocketData& data) {
	if (!data.type.empty()) {
		EventHandlerList& list = specialHandlers[data.type];
		if (!list.empty()) {
			for (EventHandler handler : list) {
				if (canceled) {
					break;
				}else{
					handler(&data);
				}
			}
		}
	}

	if (!handlers.empty()) {
		for (EventHandler handler : handlers) {
			if (canceled) {
				break;
			}else{
				handler(&data);
			}
		}
	}

	canceled = false;
}

void EventManager::Cancel() {
	canceled = true;
}

bool EventManager::IsCanceled() {
	return canceled;
}