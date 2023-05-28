#pragma once

#ifndef UTILITY_HEADER
#define UTILITY_HAEDER

#include <vector>
#include <string>
#include <map>

#define INFO_MESSAGE " INFO: "
#define ERROR_MESSAGE " ERROR: "
#define WARNING_MESSAGE " WARNING: "

#define CONSOLE_INPUT_COLOR 0x06
#define CONSOLE_OUTPUT_COLOR 0x0f

#define NET_CONFIG_WORKING_BUFFER_SIZE 16384
#define NET_CONFIG_MAX_TRIES 3

#define FILE_SENDING_BUFFER_SIZE 0xffff

typedef unsigned int uint;

typedef std::vector<std::string> StringList;
typedef std::map<std::string, std::string> StringMap;

enum ConsoleMode {
	Input,
	Output
};

struct NetAdapter {
	std::string adapter;
	std::string name;
	std::string ip;
};

typedef std::vector<NetAdapter> NetAdapterList;

std::string GetFilteredVersion(const float&);

void SplitString(const std::string&, const std::string&, StringList*);

std::string ConcatString(StringList*, const std::string&);
std::string ReplaceString(const std::string&, const std::string&, const std::string&);

int GetApplicationPath(std::string*);
int GetApplicationFileName(std::string*);
int GetApplicationDirectory(std::string*);

void GetNetAdapterList(NetAdapterList*);
bool IsValidNetAdapter(const NetAdapter&);

void GetWifiAdapter(NetAdapter*);
bool HasWifiAdapter();

void GetWindowsUsername(std::string*);

std::string GetTime();

#define CURRENT_TIME "[" + GetTime() + "]"

void SetConsoleColorMode(const ConsoleMode&);

bool FindNextPos(std::string*, const std::string&, const size_t& from = 0, size_t* pos = nullptr);

void Ignore(std::string*, const std::string&, const size_t& from = 0);

void FileProcessingCleanup(bool keepFolder = true);

#endif