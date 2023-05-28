#include "SharedEvents.h"

#include <iostream>

#include "FolderQueue.h"
#include "HostManager.h"
#include "ClientConnection.h"

#include "Utility.h"

extern HostManager* g_HostManager;
extern ClientConnection* g_ClientConnection;

extern FILE*  WorkingFile;
extern void*  FileBuffer;
extern size_t FileBytesRead;
extern size_t FileSize;
extern bool   IsProcessingFile;

extern FolderQueue* FolderInterface;

void TriggerRemoteEvent(SocketData data) {
	if (g_HostManager->IsConnected()) {
		g_HostManager->TriggerClientEvent(data);
	}else{
		g_ClientConnection->TriggerServerEvent(data);
	}
}

void Interrupt(std::string reason = std::string());
void Interrupt(std::string reason) {
	FileProcessingCleanup();

	SetConsoleColorMode(ConsoleMode::Output);
	std::cout << CURRENT_TIME << WARNING_MESSAGE << "file processing Interrupted.";
	
	if (!reason.empty()) {
		std::cout << "[reason : '" << reason << "']";
	}

	std::cout << std::endl;
	SetConsoleColorMode(ConsoleMode::Input);
	
	SocketData data;
	data.type = SHARED_EVENTS_TYPE_INTERRUPTED;
	
	if (!reason.empty()) {
		data.content = reason;
	}

	try {
		TriggerRemoteEvent(data);
	}catch (const std::exception& exception) {
		SetConsoleColorMode(ConsoleMode::Output);
		std::cout << CURRENT_TIME << ERROR_MESSAGE << "couldn't send data via socket.[error : '" << exception.what() << "']" << std::endl;
		SetConsoleColorMode(ConsoleMode::Input);
	}
}

void SharedEvents::OnSay(SocketData* data) {
	std::string username = "client";
	if (data->HasEntry("username")) {
		username = data->GetEntryValue("username");
	}
	
	SetConsoleColorMode(ConsoleMode::Output);
	std::cout << CURRENT_TIME << INFO_MESSAGE << username << " says : " << data->content << std::endl;
	SetConsoleColorMode(ConsoleMode::Input);
}

void SharedEvents::NewFile(SocketData* data) {
	if (data->HasContent()) {
		FileProcessingCleanup();

		WorkingFile = FolderQueue::CreateFileWithPath(data->content);
		if (WorkingFile) {
			SocketData sendData;
			sendData.type = SHARED_EVENTS_TYPE_REQUESTFILECHUNK;

			if (data->HasEntry("size")) {
				std::string sizeValue = data->GetEntryValue("size");
				FileSize = std::stoi(sizeValue);
			}

			IsProcessingFile = true;

			SetConsoleColorMode(ConsoleMode::Output);
			std::cout << CURRENT_TIME << INFO_MESSAGE << "started processing file[" << data->content << "]" << std::endl;
			SetConsoleColorMode(ConsoleMode::Input);
			
			try{
				TriggerRemoteEvent(sendData);
			}catch (const std::exception& exception) {
				SetConsoleColorMode(ConsoleMode::Output);
				std::cout << CURRENT_TIME << ERROR_MESSAGE << exception.what() << std::endl;
				SetConsoleColorMode(ConsoleMode::Input);
			}
		}else{
			Interrupt("couldn't create file");
		}
	}else{
		SetConsoleColorMode(ConsoleMode::Output);
		std::cout << CURRENT_TIME << ERROR_MESSAGE << "couldn't get file path to create file" << std::endl;
		SetConsoleColorMode(ConsoleMode::Input);
	}
}

void SharedEvents::RequestFileChunk(SocketData* data) {	
	if (WorkingFile) {
		memset(FileBuffer, 0, FILE_SENDING_BUFFER_SIZE);

		size_t readBytes = fread(FileBuffer, sizeof(char), FILE_SENDING_BUFFER_SIZE, WorkingFile);

		FileBytesRead += readBytes;

		double progress = ((double)FileBytesRead / (double)FileSize) * 100;

		std::cout << CURRENT_TIME << INFO_MESSAGE << "sending...[" << FileBytesRead << "/" << FileSize << " (" << progress << "%)" << "]" << std::endl;

		if (readBytes <= 0) {
			FileProcessingCleanup(false);

			if (FolderInterface) {
				std::string applicationName;
				GetApplicationFileName(&applicationName);

				while (FolderQueueEntity* entity = FolderInterface->Next()) {
					if (entity->directory.empty() && entity->name == applicationName) {
						continue;
					}

					std::string path;

					if (!entity->directory.empty()) {
						path += entity->directory;
						path += "/";
					}

					path += entity->name;

					WorkingFile = fopen(path.c_str(), "rb");
					if (WorkingFile) {
						memset(FileBuffer, 0, FILE_SENDING_BUFFER_SIZE);

						FileBytesRead = 0;

						fseek(WorkingFile, 0, SEEK_SET);
						FileSize = ftell(WorkingFile);
						rewind(WorkingFile);

						IsProcessingFile = true;

						SetConsoleColorMode(ConsoleMode::Output);
						std::cout << CURRENT_TIME << INFO_MESSAGE << "started sending file[" << path << "]" << std::endl;
						SetConsoleColorMode(ConsoleMode::Input);

						SocketData sendData;
						sendData.type = SHARED_EVENTS_TYPE_NEWFILE;
						sendData.content = path;
						sendData.entries.push_back({ "size", std::to_string(FileSize) });

						try {
							TriggerRemoteEvent(sendData);
						}catch (const std::exception& exception) {
							SetConsoleColorMode(ConsoleMode::Output);
							std::cout << CURRENT_TIME << ERROR_MESSAGE << exception.what() << std::endl;
							SetConsoleColorMode(ConsoleMode::Input);
						}

						return;
					}
				}

				FileProcessingCleanup(true);
			}

			SetConsoleColorMode(ConsoleMode::Output);
			std::cout << CURRENT_TIME << INFO_MESSAGE << "system processed all files" << std::endl;
			SetConsoleColorMode(ConsoleMode::Input);
			
			SocketData sendData;
			sendData.type = SHARED_EVENTS_TYPE_END;
			
			try {
				TriggerRemoteEvent(sendData);
			}catch (const std::exception& exception) {
				SetConsoleColorMode(ConsoleMode::Output);
				std::cout << CURRENT_TIME << ERROR_MESSAGE << exception.what() << std::endl;
				SetConsoleColorMode(ConsoleMode::Input);
			}
		}else{
			SocketData sendData;
			sendData.type = SHARED_EVENTS_TYPE_RECIEVEFILECHUNK;
			sendData.content = std::string((char*)FileBuffer, readBytes);

			try {
				TriggerRemoteEvent(sendData);
			}catch (const std::exception& exception) {
				SetConsoleColorMode(ConsoleMode::Output);
				std::cout << CURRENT_TIME << ERROR_MESSAGE << exception.what() << std::endl;
				SetConsoleColorMode(ConsoleMode::Input);
			}
		}
	}else{
		Interrupt("lost connection with file reader");
	}
}

void SharedEvents::RecieveFileChunk(SocketData* data) {
	if (data->HasContent()) {
		if (WorkingFile) {
			if (!data->content.empty()) {
				FileBytesRead += data->content.length();

				double progress = ((double)FileBytesRead / (double)FileSize) * 100;

				std::cout << CURRENT_TIME << INFO_MESSAGE << "getting...[" << FileBytesRead << "/" << FileSize << " (" << progress << "%)" << "]" << std::endl;

				fwrite(data->content.data(), sizeof(char), data->content.length(), WorkingFile);

				SocketData sendData;
				sendData.type = SHARED_EVENTS_TYPE_REQUESTFILECHUNK;

				try {
					TriggerRemoteEvent(sendData);
				}catch (const std::exception& exception) {
					SetConsoleColorMode(ConsoleMode::Output);
					std::cout << CURRENT_TIME << ERROR_MESSAGE << exception.what() << std::endl;
					SetConsoleColorMode(ConsoleMode::Input);
				}
			}
		}else{
			Interrupt("lost connection with file");
		}
	}
}

void SharedEvents::Interrupted(SocketData* data) {
	std::string reason;
	if (data->HasContent()) {
		reason = data->content;
	}

	if (WorkingFile) {
		fclose(WorkingFile);
	}

	SetConsoleColorMode(ConsoleMode::Output);
	std::cout << CURRENT_TIME << WARNING_MESSAGE << "file processing interrupted.";
	SetConsoleColorMode(ConsoleMode::Input);

	if (!reason.empty()) {
		SetConsoleColorMode(ConsoleMode::Output);
		std::cout << "[reason : '" << reason << "']";
		SetConsoleColorMode(ConsoleMode::Input);
	}

	std::cout << std::endl;

	SetConsoleColorMode(ConsoleMode::Input);
}

void SharedEvents::End(SocketData* data) {
	FileProcessingCleanup();

	SetConsoleColorMode(ConsoleMode::Output);
	std::cout << CURRENT_TIME << INFO_MESSAGE << "system processed all files." << std::endl;
	SetConsoleColorMode(ConsoleMode::Input);
}