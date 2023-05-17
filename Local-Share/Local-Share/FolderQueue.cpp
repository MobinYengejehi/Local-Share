#include "FolderQueue.h"

#include <filesystem>

#include "Utility.h"

FolderQueue::FolderQueue(const std::string& dPath) {
	if (dPath.empty()) {
		return;
	}

	path = dPath;
	directory = true;

	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(dPath)) {
		std::string name;
		
		const wchar_t* ePath = entry.path().c_str();

		const size_t pathLength = wcslen(ePath);

		char* pathPtr = new char[pathLength + 1];

		wcstombs_s(NULL, pathPtr, pathLength + 1, ePath, pathLength + 1);

		std::string fPath(pathPtr, pathLength);

		size_t start = 0;
		if (FindNextPos(&fPath, "\\", start, &start)) {
			name = fPath.substr(start + 1);

			if (!name.empty()) {
				/*FolderQueueEntry fEntry;
				fEntry.name = name;

				if (entry.is_directory()) {
					fEntry.directory = true;

					std::string fePath = dPath;
					fePath += "/";
					fePath += name;


				}*/

				FolderQueue queue(name, entry.is_directory(), ReplaceString(fPath, "\\", "/"));
				entries.push_back(queue);
			}
		}

		delete[] pathPtr;
	}

	/*std::string path;
	GetApplicationDirectory(&path);
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		std::string name;
		const wchar_t* ePath = entry.path().c_str();

		const size_t pathLength = wcslen(ePath);

		char* pathPtr = new char[pathLength + 1];

		wcstombs_s(NULL, pathPtr, pathLength + 1, ePath, pathLength + 1);

		std::string fPath(pathPtr, pathLength);

		size_t start = 0;
		FindNextPos(&fPath, "\\", start, &start);
		name = fPath.substr(start + 1);

		std::cout << "path is : " << name << std::endl;
	}*/
}

FolderQueue::FolderQueue(const std::string& fName, const bool& isdir, const std::string& lPath) {
	name = fName;
	directory = isdir;
	usedName = false;

	target = this;

	path = lPath;

	index = 0;

	if (isdir) {
		if (lPath.empty()) {
			return;
		}

		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(lPath)) {
			std::string name;
		
			const wchar_t* ePath = entry.path().c_str();

			const size_t pathLength = wcslen(ePath);

			char* pathPtr = new char[pathLength + 1];

			wcstombs_s(NULL, pathPtr, pathLength + 1, ePath, pathLength + 1);

			std::string fPath(pathPtr, pathLength);

			size_t start = 0;
			if (FindNextPos(&fPath, "\\", start, &start)) {
				name = fPath.substr(start + 1);

				if (!name.empty()) {
					FolderQueue queue(name, entry.is_directory(), ReplaceString(fPath, "\\", "/"));
					entries.push_back(queue);
				}
			}

			delete[] pathPtr;
		}
	}
}

int& FolderQueue::GetIndex() {
	return index;
}

std::string& FolderQueue::GetPath() {
	return path;
}

std::string& FolderQueue::GetName() {
	return name;
}

FolderQueue* FolderQueue::GetTarget() {
	return target;
}

FolderQueueList& FolderQueue::GetEntries() {
	return entries;
}

void FolderQueue::SetUsedName(const bool& state) {
	usedName = state;
}

void FolderQueue::SetTarget(FolderQueue* queue) {
	target = queue;
}

bool FolderQueue::Next(std::string* value) {
	if (!value) {
		return false;
	}
	if (entries.empty()) {
		return false;
	}
	if (!target) {
		return false;
	}

	if (!target->HasUsedName()) {
		if (!target->GetName().empty()) {
			target->SetUsedName(true);
			*value = target->GetName();
			return true;
		}
	}

	FolderQueue& entry = target->GetEntries()[target->GetIndex()];
	if (entry.IsDirectory()) {
		
	}else{
		*value = entry.GetName();
		target = &entry;
	}

	if (!NextIndex()) {
		target = this;
		Reset();
		return false;
	}

	return true;
	//std::cout << "called for " << name << std::endl;
	/*if (entries.empty()) {
		return false;
	}
	
	//std::cout << "start" << std::endl;

	if (!usedName) {
		if (!name.empty()) {
			usedName = true;
			//std::cout << "name used " << name << std::endl;
			*value = name;
			return true;
		}
	}

	FolderQueue& entry = entries[index];
	if (entry.IsDirectory()) {
		if (!entry.HasUsedName()) {
			std::string entryName = entry.GetName();
			if (!entryName.empty()) {
				entry.SetUsedName(true);
				target = &entry;
				return name;
			}
		}

	}else{
		if (!NextIndex()) {
			target = this;
			return std::string();
		}

		target = &entry;
		return entry.GetName();
	}*/

	/*FolderQueue& entry = entries[index];
	if (entry.IsDirectory()) {
		//std::cout << "as dir" << std::endl;
		std::string value = entry.Next();
		if (value.empty()) {
			index++;
		}
		return value;
	}else{
		if (index >= entries.size()) {
			Reset();
			//std::cout << "reset" << std::endl;
			return std::string();
		}
		index++;
		//std::cout << "next item" << std::endl;
		return entry.GetName();
	}*/

	//return std::string();
}

bool FolderQueue::NextIndex() {
	if (index >= entries.size()) {
		index = 0;
		return false;
	}

	index++;
	return true;
}

void FolderQueue::Reset() {
	index = 0;
	usedName = false;
}

bool FolderQueue::IsDirectory() {
	return directory;
}

bool FolderQueue::HasUsedName() {
	return usedName;
}