#pragma once

#ifndef FOLDER_QUEUE_HEADER
#define FOLDER_QUEUE_HEADER

#include <string>
#include <vector>

struct FolderQueueEntity {
	std::string directory;
	std::string name;
};

typedef std::vector<FolderQueueEntity> FolderQueueEntityList;

class FolderQueue {
public:
	FolderQueue(const std::string&);
	~FolderQueue() = default;

	FolderQueueEntityList& GetEntities();
	size_t                 GetEntitiesCount();

	void SetEntities(const FolderQueueEntityList&);

	FolderQueueEntity* Next();

	bool IsEmpty();

	static FILE* CreateFileWithPath(const std::string& path, std::string on = "~");

private:
	int                   on;
	FolderQueueEntityList entities;
};

#endif