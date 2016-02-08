#pragma once
#include "MemoryStatistics.h"
#include <thread>


#define ENABLE_MEMORY_TRACKER

namespace std
{
	class recursive_mutex;
}

namespace Prism
{
	enum class eMemoryType
	{
		UNKNOWN,
		NEW,
		NEW_ARRAY,
		MALLOC
	};

	struct MemoryData
	{

		MemoryData()
			: myFileName(""), myFunctionName(""), myLine(-1), myType(eMemoryType::UNKNOWN), myAddress(nullptr), myBytes(0) {}

		void* myAddress;
		size_t myBytes;
		const char* myFileName;
		const char* myFunctionName;
		int myLine;
		eMemoryType myType;
	};

	class MemoryTracker
	{
#ifdef ENABLE_MEMORY_TRACKER
		friend void* ::operator new(size_t aBytes);
		friend void* ::operator new[](size_t aBytes);
		friend void ::operator delete(void* aAddress);
		friend void ::operator delete[](void* aAddress);
#endif
	public:
		static void Destroy();
		static MemoryTracker* GetInstance();

		void Allocate(int aLine, const char* aFile, const char* aFunction);
		void Free(void* aAddress);

		void SetRunTime(bool aStatus);
		bool GetRunTime() const;
		void AllowNewDuringRunTime(const std::thread::id& aThreadId);
		void DumpToFile();

	private:
		static void Create();
		MemoryTracker();
		~MemoryTracker();

		void Add(void* aAddress, size_t aBytes, eMemoryType aMemoryType);
		void Remove(void* aAddress, bool aLock = true);

		MemoryStatistics myMemoryStatistics;
		MemoryData myTopicalData;
		MemoryData* myData;
		int myAllocations;
		bool myRuntime;

		std::recursive_mutex* myMutex;

		static MemoryTracker* myInstance;
		std::thread::id myAllowThread;
	};

	inline void MemoryTracker::SetRunTime(bool aStatus)
	{
		myRuntime = aStatus;
	}

	inline bool MemoryTracker::GetRunTime() const
	{
		return myRuntime;
	}

	inline void MemoryTracker::AllowNewDuringRunTime(const std::thread::id& aThreadId)
	{
		myAllowThread = aThreadId;
	}
}