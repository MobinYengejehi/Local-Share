#pragma warning(disable : 4996)

#include "Utility.h"

#include <ctime>
#include <Windows.h>

std::string GetFilteredVersion(const float& Version) {
	std::string version;
	char formater[4];
	sprintf(formater, "%.2f", Version);
	version += formater[0];
	version += ".";
	version += formater[2];
	version += ".";
	version += formater[3];
	return version;
}

void SplitString(const std::string& input, const std::string& seperator, std::vector<std::string>* list) {
	if (input.empty() || seperator.empty()) {
		return;
	}
	std::string chunk;
	size_t length = input.length();
	size_t sepLength = seperator.length();
	for (size_t i = 0; i < length; i++) {
		if (input.substr(i, sepLength) == seperator) {
			list->push_back(chunk);
			chunk.clear();
			i += sepLength - 1;
			continue;
		}
		chunk += input[i];
	}
	list->push_back(chunk);
}

std::string ConcatString(StringList* list, const std::string& space) {
	std::string result;
	if (!list->empty()) {
		for (std::string& string : *list) {
			result += string + space;
		}
		result = result.substr(0, result.length() - space.length());
	}
	return result;
}

std::string ReplaceString(const std::string& input, const std::string& chunk, const std::string& with) {
	if (!input.empty() && !chunk.empty() && !with.empty()) {
		StringList list;
		SplitString(input, chunk, &list);
		return ConcatString(&list, with);
	}
	return input;
}

int GetApplicationPath(std::string* path) {
	char pathBuffer[256];
	int bytes = GetModuleFileNameA(NULL, pathBuffer, sizeof(pathBuffer));
	*path = ReplaceString(pathBuffer, "\\", "/");
	return bytes ? bytes : -1;
}

int GetApplicationDirectory(std::string* directory) {
	std::string path;
	int bytes = GetApplicationPath(&path);
	if (bytes != 0) {
		StringList list;
		SplitString(path, "/", &list);
		list.pop_back();
		*directory = ConcatString(&list, "/");
		return bytes;
	}
	return -1;
}

std::string GetTime() {
	std::time_t time = std::time(0);
	std::tm* now = std::localtime(&time);
	char formater[0xa];
	sprintf(formater, "%02d:%02d:%02d", now->tm_hour, now->tm_min, now->tm_sec);
	std::string result(formater);
	return result;
}

void SetConsoleColorMode(const ConsoleMode& mode) {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (mode == ConsoleMode::Input) {
		SetConsoleTextAttribute(consoleHandle, CONSOLE_INPUT_COLOR);
	}else if (mode == ConsoleMode::Output) {
		SetConsoleTextAttribute(consoleHandle, CONSOLE_OUTPUT_COLOR);
	}
}

bool FindNextPos(std::string* string, const std::string& what, const size_t& from, size_t* position) {
	if (!string) {
		return false;
	}
	if (what.empty()) {
		return false;
	}

	size_t length = string->length();
	size_t whatLength = what.length();

	if (length < 1) {
		return false;
	}
	if (whatLength < 1) {
		return false;
	}

	for (size_t i = from; i < length; i++) {
		if (string->substr(i, whatLength) == what) {
			if (position) {
				*position = i;
			}
			return true;
		}
	}

	return false;
}

void Ignore(std::string* string, const std::string& what, const size_t& from) {
	if (!string) {
		return;
	}
	if (what.empty()) {
		return;
	}

	size_t length = string->length();
	size_t whatLength = what.length();

	if (length < 1) {
		return;
	}
	if (whatLength < 1) {
		return;
	}

	const char* data = string->data();
	const char* whatData = what.data();

	std::string result;
	for (size_t i = from; i < length; i++) {
		char byte = data[i];

		bool found = false;
		for (size_t j = 0; j < whatLength; j++) {
			char wByte = whatData[j];
			if (byte == wByte) {
				found = true;
				break;
			}
		}

		if (!found) {
			result += byte;
		}
	}

	*string = result;
}