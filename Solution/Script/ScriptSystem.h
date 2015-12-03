#pragma once

#include <functional>
#include "LuaFiles\lua.hpp"
#include <unordered_map>
#include <vector>
#include "LuaArgs.h"
#include "FileWatcher.h"
#include <fstream>
namespace LUA
{
	enum class eFunctionStatus
	{
		NOT_FOUND,
		INVALID_ARGS,
		OK,
	};

	class ScriptSystem
	{
	public:
		static void Create();
		static ScriptSystem* GetInstance();
		static void Destroy();

		void Init(const std::function<void()>& aRegisterFunction);

		void RegisterFunction(const std::string& aNameInLua, lua_CFunction aFunction
			, const std::string& aArguments, const std::string& aHelpText);

		eFunctionStatus CallFunction(const std::string& aFunctionName, const LuaArguments& someArgs);
		void UseFile(const std::string& aFileName);

		void Update();
		void PrintToFile(const std::string& aString);
	private:
		ScriptSystem();
		~ScriptSystem();

		struct Documentation
		{
			std::string myFunction;
			std::string myArguments;
			std::string myHelpText;
		};

		void ReInit(const std::string& aString);
		void CheckError(int aCode);
		void CheckFunctionError(int aCode);

		void PushArg(const Arg& aArg);
		void AddLuaFunction(const std::string& aNameInLua, int aNumberOfArgs);

		void PrintDocumentation();
		int levenshtein_distance(const std::string &s1, const std::string &s2);

		std::string FindClosestFunction(const std::string& aFailedFunction);

		std::unordered_map<std::string, int> myArgumentsCount;
		std::function<void()> myCppRegisterFunction;
		std::vector<std::string> myActiveFiles;
		std::vector<Documentation> myDocumentation;

		lua_State* myLuaState;
		FileWatcher myFileWatcher;
		std::fstream myOutputFile;
		static ScriptSystem* myInstance;
	};
}