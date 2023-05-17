#pragma warning(disable : 4996)

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include "Utility.h"

#include <ctime>
#include <Windows.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <lmcons.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

#define NET_CONFIG_WIFI_NAME "Wi-Fi"

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

void GetNetAdapterList(NetAdapterList* list) {
	if (!list) {
		return;
	}

	IP_ADAPTER_ADDRESSES* addresses = NULL;
	IP_ADAPTER_ADDRESSES* adapter = NULL;

	DWORD bufferSize = NET_CONFIG_WORKING_BUFFER_SIZE;
	
	for (int attempts = 0; attempts < NET_CONFIG_MAX_TRIES; attempts++) {
		addresses = (IP_ADAPTER_ADDRESSES*)malloc(bufferSize);

		DWORD error = GetAdaptersAddresses(
			AF_UNSPEC,
			GAA_FLAG_SKIP_ANYCAST |
			GAA_FLAG_SKIP_MULTICAST |
			GAA_FLAG_SKIP_DNS_SERVER |
			GAA_FLAG_SKIP_FRIENDLY_NAME,
			NULL,
			addresses,
			&bufferSize
		);

		if (error == ERROR_SUCCESS) {
			break;
		}else if (error == ERROR_BUFFER_OVERFLOW) {
			free(addresses);
			addresses = NULL;
			continue;
		}else{
			free(addresses);
			addresses = NULL;
			return;
		}
	}

	for (adapter = addresses; adapter != NULL; adapter = adapter->Next) {
		if (adapter->IfType == IF_TYPE_SOFTWARE_LOOPBACK) {
			continue;
		}

		const size_t descLength = wcslen(adapter->Description);
		const size_t nameLength = wcslen(adapter->FriendlyName);

		char* description = new char[descLength + 1];
		char* name = new char[nameLength + 1];

		wcstombs_s(NULL, description, descLength + 1, adapter->Description, descLength + 1);
		wcstombs_s(NULL, name, nameLength + 1, adapter->FriendlyName, nameLength + 1);

		NetAdapter netAddress;
		netAddress.adapter = description;
		netAddress.name = name;

		delete[] description;
		delete[] name;

		for (IP_ADAPTER_UNICAST_ADDRESS* address = adapter->FirstUnicastAddress; address != NULL; address = address->Next) {
			ADDRESS_FAMILY family = address->Address.lpSockaddr->sa_family;
			if (family == AF_INET) {
				SOCKADDR_IN* ipv4 = reinterpret_cast<SOCKADDR_IN*>(address->Address.lpSockaddr);

				char info[16] = {NULL};
				inet_ntop(AF_INET, &(ipv4->sin_addr), info, 16);

				netAddress.ip = info;
			}
		}

		list->push_back(netAddress);
	}

	free(addresses);
	addresses = NULL;
}

bool IsValidNetAdapter(const NetAdapter& adapter) {
	return (
		!adapter.adapter.empty() &&
		!adapter.name.empty() &&
		!adapter.ip.empty()
	);
}

void GetWifiAdapter(NetAdapter* adapter) {
	if (!adapter) {
		return;
	}

	NetAdapterList addressList;
	GetNetAdapterList(&addressList);

	if (addressList.empty()) {
		return;
	}

	for (NetAdapter& addr : addressList) {
		if (addr.name == NET_CONFIG_WIFI_NAME) {
			*adapter = addr;
			return;
		}
	}
}

void GetWindowsUsername(std::string* username) {
	char  user[UNLEN + 1];
	DWORD usernameLength = UNLEN + 1;

	GetUserName(user, &usernameLength);

	*username = std::string(user, usernameLength - 1);
}

bool HasWifiAdapter() {
	NetAdapterList addressList;
	GetNetAdapterList(&addressList);

	if (addressList.empty()) {
		return false;
	}

	for (NetAdapter& address : addressList) {
		if (address.name == NET_CONFIG_WIFI_NAME) {
			return true;
		}
	}
	return false;
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