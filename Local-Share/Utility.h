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

typedef unsigned int uint;

typedef std::vector<std::string> StringList;
typedef std::map<std::string, std::string> StringMap;

enum ConsoleMode {
	Input,
	Output
};

std::string GetFilteredVersion(const float&);

void SplitString(const std::string&, const std::string&, StringList*);

std::string ConcatString(StringList*, const std::string&);
std::string ReplaceString(const std::string&, const std::string&, const std::string&);

int GetApplicationPath(std::string*);
int GetApplicationDirectory(std::string*);

std::string GetTime();

#define CURRENT_TIME "[" + GetTime() + "]"

void SetConsoleColorMode(const ConsoleMode&);

bool FindNextPos(std::string*, const std::string&, const size_t& from = 0, size_t* pos = nullptr);

void Ignore(std::string*, const std::string&, const size_t& from = 0);

#endif