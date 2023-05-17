#pragma once

#ifndef FOLDER_QUEUE_HEADER
#define FOLDER_QUEUE_HEADER

#include <string>
#include <vector>

/*class FolderQueueEntry;
class FolderQueue;

typedef std::vector<FolderQueueEntry> FolderQueueEntryList;

class FolderQueueEntry {
public:
	std::string name;
	bool        directory;
	FolderQueue entries;

	FolderQueueEntry();
};*/

class FolderQueue;

typedef std::vector<FolderQueue> FolderQueueList;

class FolderQueue {
public:
	FolderQueue(const std::string&);
	FolderQueue(const std::string&, const bool&, const std::string&);
	~FolderQueue() = default;

	int&         GetIndex();
	std::string& GetPath();
	std::string& GetName();

	FolderQueue* GetTarget();

	FolderQueueList& GetEntries();

	void SetUsedName(const bool&);
	void SetTarget(FolderQueue*);

	bool Next(std::string*);
	bool NextIndex();
	void Reset();

	bool IsDirectory();
	bool HasUsedName();
	
private:
	int         index;
	bool        directory;
	bool        usedName;
	std::string path;
	std::string name;

	FolderQueue* target;

	FolderQueueList entries;
};

#endif