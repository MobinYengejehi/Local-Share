#include "SocketData.h"

#include "Utility.h"

#define SOCKET_DATA_ENTRY_SEPRATOR ","
#define SOCKET_DATA_ENTRY_EQUAL ":"
#define SOCKET_DATA_ENTRY_BRACKET "\""

#define SOCKET_DATA_SEPRATOR ";"

SocketDataEntry::operator std::string() {

	std::string result = SOCKET_DATA_ENTRY_BRACKET;
	result += key;
	result += SOCKET_DATA_ENTRY_BRACKET;
	result += SOCKET_DATA_ENTRY_EQUAL;
	result += SOCKET_DATA_ENTRY_BRACKET;
	result += value;
	result += SOCKET_DATA_ENTRY_BRACKET;
	
	return result;
}

SocketData::SocketData(const std::string& data) {
	Parse(this, data);
}

std::string SocketData::GetEntryValue(const std::string& key) {
	if (key.empty()) {
		return std::string();
	}
	if (entries.empty()) {
		return std::string();
	}

	for (SocketDataEntry& entry : entries) {
		if (entry.key == key) {
			return entry.value;
		}
	}
	return std::string();
}

SocketDataEntry SocketData::GetEntry(const std::string& key) {
	if (key.empty()) {
		return SocketDataEntry();
	}
	if (entries.empty()) {
		return SocketDataEntry();
	}

	for (SocketDataEntry& entry : entries) {
		if (entry.key == key) {
			return entry;
		}
	}
	return SocketDataEntry();
}

bool SocketData::HasEntry(const std::string& key) {
	if (key.empty()) {
		return false;
	}
	if (entries.empty()) {
		return false;
	}

	for (SocketDataEntry& entry : entries) {
		if (entry.key == key) {
			return true;
		}
	}
	return false;
}

bool SocketData::HasContent() {
	return !content.empty();
}

SocketData::operator std::string() {
	std::string result = type;
	result += SOCKET_DATA_SEPRATOR;

	if (!entries.empty()) {
		for (SocketDataEntry& entry : entries) {
			result += (std::string)entry;
			result += SOCKET_DATA_ENTRY_SEPRATOR;
		}
		result.pop_back();
		result += SOCKET_DATA_SEPRATOR;
	}

	result += content;

	return result;
}

void SocketData::Parse(SocketData* data, std::string buffer) {
	if (!data) {
		return;
	}
	if (buffer.empty()) {
		return;
	}

	size_t start = 0;
	size_t to = 0;

 	if (FindNextPos(&buffer, SOCKET_DATA_SEPRATOR, start, &to)) {
		data->type = buffer.substr(start, to - start);
		start = to + 1;

		if (FindNextPos(&buffer, SOCKET_DATA_SEPRATOR, start, &to)) {
			std::string entriesBuffer = buffer.substr(start, to - start);
			if (!entriesBuffer.empty()) {
				size_t length = entriesBuffer.length();

				char byte = entriesBuffer[0];
				char bracketByte = std::string(SOCKET_DATA_ENTRY_BRACKET)[0];
				char sepratorByte = std::string(SOCKET_DATA_ENTRY_SEPRATOR)[0];

				std::string key;
				std::string value;

				bool inside = false;
				bool fromKey = false;

				for (size_t i = 0; i < length; byte = entriesBuffer[++i]) {
					if (inside) {
						if (byte == bracketByte) {
							if (!fromKey) {
								data->entries.push_back({key, value});
							}
							inside = false;
							fromKey = false;
						}else{
							(fromKey ? key : value).push_back(byte);
						}
						continue;
					}
					if (byte == sepratorByte) {
						key = std::string();
						value = std::string();
						continue;
					}
					if (key.empty()) {
						if (byte == bracketByte) {
							fromKey = true;
							inside = true;
						}
					}else if (value.empty()) {
						if (byte == bracketByte) {
							inside = true;
						}
					}else{
						data->entries.push_back({key, value});
					}
				}
			}

			start = to + 1;
			data->content = buffer.substr(start);
		}else{
			data->content = buffer.substr(start);
		}
	}else{
		data->type = buffer;
	}
}