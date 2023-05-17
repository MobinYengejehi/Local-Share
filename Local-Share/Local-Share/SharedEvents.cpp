#include "SharedEvents.h"

#include <iostream>

void SharedEvents::OnSay(SocketData* data) {
	std::string username = "client";
	if (data->HasEntry("username")) {
		username = data->GetEntryValue("username");
	}
	
	SetConsoleColorMode(ConsoleMode::Output);
	std::cout << CURRENT_TIME << INFO_MESSAGE << username << " says : " << data->content << std::endl;
	SetConsoleColorMode(ConsoleMode::Input);
}

void SharedEvents::OnCreateFile(SocketData* data) {

}

void SharedEvents::OnCreateDirectory(SocketData* data) {

}

void SharedEvents::OnRecieveFileData(SocketData* data) {

}

void SharedEvents::OnGotChunk(SocketData* data) {

}

void SharedEvents::OnRequestNextFile(SocketData* data) {

}