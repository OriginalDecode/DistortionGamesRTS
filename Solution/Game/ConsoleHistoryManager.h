#pragma once
#include <GrowingArray.h>

class ConsoleHistoryManager
{
public:
	ConsoleHistoryManager();
	~ConsoleHistoryManager();

	void GetNext();
	void GetPrevious();
	const std::string& GetCurrent();
	void AddHistory(const std::string& aCommand);
	void Save();
	void Load();

private:

	CU::GrowingArray<std::string> myHistory;
	int myCurrentIndex;

	const char* myHistoryFile;
	int myInsertIndex;

};
