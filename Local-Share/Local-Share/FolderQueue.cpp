#include "FolderQueue.h"

#include <filesystem>

#include "Utility.h"

#include <fstream>

void SearchFiles(const std::string& path, FolderQueueEntityList* list, std::string dir) {
	const size_t pathLength = path.length();

	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path)) {
		std::string name;

		const wchar_t* ePath = entry.path().c_str();

		const size_t length = wcslen(ePath);

		char* pathString = new char[length + 1];

		wcstombs_s(NULL, pathString, length + 1, ePath, length + 1);

		std::string fPath(pathString, length);

		size_t start = 0;
		if (FindNextPos(&fPath, "\\", start, &start)) {
			name = fPath.substr(start + 1);

			if (entry.is_directory()) {
				std::string newPath = path;
				newPath += "/";
				newPath += name;

				std::string newDir = dir;
				if (!dir.empty()) newDir += "/";
				newDir += name;

				SearchFiles(newPath, list, newDir);
			}else{
				if (!name.empty()) {
					FolderQueueEntity entity;
					entity.name = name;
					entity.directory = dir;

					list->push_back(entity);
				}
			}
		}

		delete[] pathString;
	}
}

FolderQueue::FolderQueue(const std::string& path) {
	if (path.empty()) {
		return;
	}

	on = 0;

	SearchFiles(path, &entities, "");
}

FolderQueueEntityList& FolderQueue::GetEntities() {
	return entities;
}

size_t FolderQueue::GetEntitiesCount() {
	return entities.size();
}

void FolderQueue::SetEntities(const FolderQueueEntityList& list) {
	entities = list;
}

FolderQueueEntity* FolderQueue::Next() {
	if (entities.empty()) {
		return NULL;
	}

	if (on >= entities.size()) {
		on = 0;
		return NULL;
	}

	return &entities[on++];
}

bool FolderQueue::IsEmpty() {
	return entities.empty();
}

FILE* FolderQueue::CreateFileWithPath(const std::string& path, std::string on) {
	if (path.empty()) {
		return NULL;
	}

	if (on.empty() || on == "~") {
		GetApplicationDirectory(&on);
	}

	StringList directories;
	SplitString(path, "/", &directories);
	
	std::string fileName = directories.back();
	directories.pop_back();

	if (!directories.empty()) {
		for (std::string& directory : directories) {
			on += "/";
			on += directory;

			if (!std::filesystem::is_directory(on)) {
				std::filesystem::create_directory(on);
			}
		}
	}

	std::string filePath = on;
	filePath += "/";
	filePath += fileName;

	remove(filePath.c_str());

	return fopen(filePath.c_str(), "wb");
}