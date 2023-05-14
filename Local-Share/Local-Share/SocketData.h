#pragma once

#ifndef SOCKET_DATA_HEADER
#define SOCKET_DATA_HEADER

#include <string>
#include <vector>

struct SocketDataEntry {
	std::string key;
	std::string value;

	operator std::string();
};

typedef std::vector<SocketDataEntry> SocketDataEntryList;

struct SocketData {
	std::string         type;
	std::string         content;
	SocketDataEntryList entries;

	SocketData() = default;
	SocketData(const std::string&);

	std::string     GetEntryValue(const std::string&);
	SocketDataEntry GetEntry(const std::string&);

	bool HasEntry(const std::string&);
	bool HasContent();

	operator std::string();

	static void Parse(SocketData*, std::string);
};

#endif