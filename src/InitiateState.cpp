#include "http/http.h"
#include "Lua/CustomLua.h"
#include "Game.h"

#include <filesystem>


// Code taken from LuaJIT 2.1.0-beta2
namespace pd2hook
{
	void validate_mod_directories()
	{
		if (!std::filesystem::exists("mods"))
			std::filesystem::create_directory("mods");

		if (!std::filesystem::exists("mods/downloads"))
			std::filesystem::create_directory("mods/downloads");

		if (!std::filesystem::exists("mods/logs"))
			std::filesystem::create_directory("mods/logs");

		if (!std::filesystem::exists("mods/saves"))
			std::filesystem::create_directory("mods/saves");
	}

	void InitiateStates()
	{
		validate_mod_directories();

		FunctionHookManager::Setup();

		lua_init(register_lua_functions);
		init_game();

		std::ifstream infile("mods/developer.txt");

		if (infile.good())
			Logger::Instance().OpenConsole();
	}

	void DestroyStates()
	{
		FunctionHookManager::DestroyAll();

		HTTPManager::Destroy();

		Logger::DestroyInstance();
	}
}
