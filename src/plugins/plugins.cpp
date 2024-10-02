#include <stdint.h>
#include <string.h>

#include "plugins.h"
#include "util/util.h"
#include "Lua/Lua.h"

using namespace std;

namespace blt
{
	namespace plugins
	{

		list<Plugin*> plugins_list;

		PluginLoadResult LoadPlugin(string file, Plugin** out_plugin)
		{
			if (out_plugin)
				*out_plugin = NULL;

			// Major TODO before this is publicly released as stable:
			// Add some kind of security system to avoid loading untrusted plugins
			// (particularly those being used for the purpose of hiding a mod's
			//   source code - doing so is against the GPLv3 license SuperBLT is
			//   licensed under, but just in case)
			// Also require an exported function confirming GPL compliance, to make
			// plugin authors aware of the license - this could also provide a URL for
			// obtaining the plugin source code.

			for (Plugin* plugin : plugins_list)
			{
				// TODO use some kind of ID or UUID embedded into the binary for identification, not filename
				if (file == plugin->GetFile())
				{
					if (out_plugin)
						*out_plugin = plugin;

					return plr_AlreadyLoaded;
				}
			}

			PD2HOOK_LOG_LOG(string("Loading binary extension ") + file);

			try
			{
				Plugin* plugin = CreateNativePlugin(file);
				plugins_list.push_back(plugin);

				// Set up the already-running states
				RegisterPluginForActiveStates(plugin);

				if (out_plugin)
					*out_plugin = plugin;
			}
			catch (const char* err)
			{
				throw string(err);
			}

			return plr_Success;
		}

		const list<Plugin*>& GetPlugins()
		{
			return plugins_list;
		}

		Plugin::Plugin(std::string file) : file(file)
		{
		}

		void Plugin::Init()
		{
			// Version compatibility.
			uint64_t* SBLT_API_REVISION = (uint64_t*)ResolveSymbol("SBLT_API_REVISION");
			if (!SBLT_API_REVISION) throw string("Missing export SBLT_API_REVISION");

			switch (*SBLT_API_REVISION)
			{
			case 1:
				// Nothing special for now.
				break;
			default:
				throw string("Unsupported revision ") + to_string(*SBLT_API_REVISION) + " - you probably need to update RaidBLT";
			}

			// Verify the licence compliance.
			const char* const* MODULE_LICENCE_DECLARATION = (const char* const*)ResolveSymbol("MODULE_LICENCE_DECLARATION");
			if (!MODULE_LICENCE_DECLARATION) throw string("Licence error: Missing export MODULE_LICENCE_DECLARATION");

			const char* const* MODULE_SOURCE_CODE_LOCATION = (const char* const*)ResolveSymbol("MODULE_SOURCE_CODE_LOCATION");
			if (!MODULE_SOURCE_CODE_LOCATION) throw string("Licence error: Missing export MODULE_SOURCE_CODE_LOCATION");

			const char* const* MODULE_SOURCE_CODE_REVISION = (const char* const*)ResolveSymbol("MODULE_SOURCE_CODE_REVISION");
			if (!MODULE_SOURCE_CODE_REVISION) throw string("Licence error: Missing export MODULE_SOURCE_CODE_REVISION");

			const char* required_declaration = "This module is licenced under the GNU GPL version 2 or later, or another compatible licence";

			if (strcmp(required_declaration, *MODULE_LICENCE_DECLARATION))
			{
				ostringstream msg;
				msg << "Invalid licence declaration ";
				msg << "'" << (*MODULE_LICENCE_DECLARATION) << "'";
				msg << " (actual) vs ";
				msg << "'" << required_declaration << "' (correct)";
				throw msg.str();
			}

			bool developer = false;
			if (*MODULE_SOURCE_CODE_LOCATION)
			{
				// TODO handle this, put it somewhere accessable by Lua.
			}
			else
			{
				// TODO also make Lua aware of this.
				developer = true;
			}

			if (developer)
			{
				PD2HOOK_LOG_WARN("Loading development plugin! This should never occur ourside a development enviornment");
			}

			setup_state = (setup_state_func_t)ResolveSymbol("RaidBLT_Plugin_Init_State");
			if (!setup_state) throw string("Invalid dlhandle - missing setup_state func!");

			update_func = (update_func_t)ResolveSymbol("RaidBLT_Plugin_Update");
			push_lua = (push_lua_func_t)ResolveSymbol("RaidBLT_Plugin_PushLua");
		}

		void Plugin::AddToState(lua_State* L)
		{
			setup_state(L);
		}

		void Plugin::Update(lua_State* L)
		{
			if (update_func)
				update_func(L);
		}

		int Plugin::PushLuaValue(lua_State* L)
		{
			if (!push_lua)
				return 0;

			return push_lua(L);
		}
	}
}

// Don't use these funcdefs when porting to GNU+Linux, as it doesn't need
// any kind of getter function because the PD2 binary isn't stripped
typedef void* (*lua_access_func_t)(const char*);
typedef void(*init_func_t)(lua_access_func_t get_lua_func_by_name);

static void raid_log(const char* message, int level, const char* file, int line)
{
	char buffer[256];
	sprintf_s(buffer, sizeof(buffer), "ExtModDLL %s", file);

	switch ((LogType)level)
	{
	case LogType::Trace:
	case LogType::Debug:
	case LogType::Log:
	case LogType::Lua:
		PD2HOOK_LOG_LEVEL(message, (LogType)level, buffer, line, FOREGROUND_RED, FOREGROUND_BLUE, FOREGROUND_INTENSITY);
		break;
	case LogType::Warn:
		PD2HOOK_LOG_LEVEL(message, (LogType)level, buffer, line, FOREGROUND_RED, FOREGROUND_GREEN, FOREGROUND_INTENSITY);
		break;
	case LogType::Error:
		PD2HOOK_LOG_LEVEL(message, (LogType)level, buffer, line, FOREGROUND_RED, FOREGROUND_INTENSITY);
		break;
	}
}

static bool is_active_state(lua_State* L)
{
	return check_active_state(L);
}

static HMODULE hModule = nullptr;

static HMODULE GetCurrentModuleHandle() {
	if (hModule == nullptr)
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCTSTR>(GetCurrentModuleHandle), &hModule);
	return hModule;
}

static void* get_lua_func(const char* name)
{
	// Only allow getting the Lua functions
	if (strncmp(name, "lua", 3)) return NULL;

	// Don't allow getting the setup functions
	if (!strncmp(name, "luaL_newstate", 13)) return NULL;

	return GetProcAddress(GetCurrentModuleHandle(), name);
}

static void* get_func(const char* name)
{
	string str = name;

	if (str == "raid_log")
	{
		return &raid_log;
	}
	else if (str == "is_active_state")
	{
		return &is_active_state;
	}

	return get_lua_func(name);
}

class WindowsPlugin : public blt::plugins::Plugin {
public:
	WindowsPlugin(std::string file);
protected:
	virtual void* ResolveSymbol(std::string name) const;
private:
	HMODULE module;
};

WindowsPlugin::WindowsPlugin(std::string file) : blt::plugins::Plugin(file)
{
	module = LoadLibraryA(file.c_str());

	if (!module) throw string("Failed to load module: ERR") + to_string(GetLastError());

	Init();

	// Start loading everything
	init_func_t init = (init_func_t)GetProcAddress(module, "RaidBLT_Plugin_Setup");
	if (!init) throw "Invalid module - missing initfunc!";

	init(get_func);
}

void* WindowsPlugin::ResolveSymbol(std::string name) const
{
	return GetProcAddress(module, name.c_str());
}

blt::plugins::Plugin* blt::plugins::CreateNativePlugin(std::string file)
{
	return new WindowsPlugin(file);
}
