#include "InitState.h"
#include <Windows.h>

#include "signatures/signatures.h"
#include "util/util.h"
#include "console/console.h"
#include "threading/queue.h"
#include "http/http.h"

#include <thread>
#include <list>
#include <fstream>

// Code taken from LuaJIT 2.1.0-beta2
namespace pd2hook
{
	struct lua_State;

	typedef const char * (*lua_Reader) (lua_State *L, void *ud, size_t *sz);
	typedef int(*lua_CFunction) (lua_State *L);
	typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize);
	typedef struct luaL_Reg {
		const char* name;
		lua_CFunction func;
	} luaL_Reg;

	// From src/luaconf.h
#define LUA_NUMBER		double

	// From src/lua.h
	// type of numbers in Lua
	typedef LUA_NUMBER lua_Number;
	typedef struct lua_Debug lua_Debug;	// activation record
	// Functions to be called by the debuger in specific events
	typedef void(*lua_Hook) (lua_State* L, lua_Debug* ar);

	// From src/luaconf.h
#define LUA_NUMBER		double

	// From src/lua.h
	// type of numbers in Lua
	typedef LUA_NUMBER lua_Number;
	typedef struct lua_Debug lua_Debug;	// activation record
	// Functions to be called by the debuger in specific events
	typedef void(*lua_Hook) (lua_State* L, lua_Debug* ar);

	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_call, void, "\x48\x63\xC2\x4C\x8B\xD1\x48\x8B\x51\x28\x48\xC1\xE0\x03\x4C\x8B", "xxxxxxxxxxxxxxxx", 0, lua_State*, int, int)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pcall, int, "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x48\x8B\x59\x10\x41\x8B\xF0\x4C\x63\xDA\x4C\x8B\xD1\x0F\xB6\xBB\xC1", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, int, int, int)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_gettop, int, "\x48\x8B\x41\x28\x48\x2B\x41\x20\x48\xC1\xF8\x03\xC3", "xxxxxxxxxxxxx", 0, lua_State*)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_settop, void, "\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\x85\xD2\x78\x00\x4C\x8B\x41", "xxxxxxxxxxxx?xxx", 0, lua_State*, int)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_toboolean, int, "\x48\x83\xEC\x28\xE8\x00\x00\x00\x00\x48\x8B\x08\x33\xC0\x48\xC1", "xxxxx????xxxxxxx", 0, lua_State*, int)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_tointeger, ptrdiff_t, "\x48\x83\xEC\x28\xE8\x00\x00\x00\x00\x48\x8B\x10\x48\x8B\xCA\x48\xC1\xF9\x2F\x83\xF9\xF2\x00\x00\xF2\x0F\x10\x00\xF2\x48\x0F", "xxxxx????xxxxxxxxxxxxx??xxxxxxx", 0, lua_State*, int)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_tonumber, lua_Number, "\x48\x83\xEC\x28\xE8\x00\x00\x00\x00\x48\x8B\x10\x48\x8B\xCA\x48\xC1\xF9\x2F\x83\xF9\xF2\x00\x00\xF2\x0F\x10\x00\x48\x83\xC4", "xxxxx????xxxxxxxxxxxxx??xxxxxxx", 0, lua_State*, int)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_tolstring, const char*, "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x49\x8B\xF8\x8B\xDA\x48\x8B\xF1", "xxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, int, size_t*)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_objlen, size_t, "\x40\x53\x48\x83\xEC\x20\x4C\x8B\xD9\xE8\x00\x00\x00\x00\x48\x8B", "xxxxxxxxxx????xx", 0, lua_State*, int)
	// This is actually luaL_loadfilex() (as per Lua 5.2) now. The new parameter corresponds to mode, and specifying NULL causes Lua
	// to default to "bt", i.e. 'binary and text'
	// https://www.lua.org/manual/5.2/manual.html#luaL_loadfilex
	// https://www.lua.org/manual/5.2/manual.html#pdf-load
	CREATE_NORMAL_CALLABLE_SIGNATURE(luaL_loadfilex, int, "\x48\x89\x5C\x24\x20\x55\x56\x57\x48\x81\xEC\x50\x02\x00\x00\x48", "xxxxxxxxxxxxxxxx", 0, lua_State*, const char*, const char*)
	CREATE_NORMAL_CALLABLE_SIGNATURE(luaL_loadstring, int, "\x48\x83\xEC\x48\x48\x89\x54\x24\x30\x48\x83\xC8\xFF\x0F\x1F\x00", "xxxxxxxxxxxxxxxx", 0, lua_State*, const char*)
	//CREATE_CALLABLE_SIGNATURE(lua_load, int, "\x48\x89\x5C\x24\x10\x57\x48\x83\xEC\x20\x4D\x8B\xD8\x48\x8B\xD9", "xxxxxxxxxxx", 0, lua_State*, lua_Reader, void*, const char*)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_getfield, void, "\x48\x89\x5C\x24\x10\x57\x48\x83\xEC\x20\x4D\x8B\xD8\x48\x8B\xD9", "xxxxxxxxxxxxxxxx", 0, lua_State*, int, const char*)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_setfield, int, "\x48\x89\x5C\x24\x08\x57\x48\x83\xEC\x20\x4D\x8B\xD8\x48\x8B\xD9", "xxxxxxxxxxxxxxxx", 0, lua_State*, int, const char*)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_createtable, void, "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x4C\x8B\x49\x10\x41\x8B\xF8\x8B\xF2\x48\x8B\xD9\x49\x8B\x41\x28\x49", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, int, int)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_insert, void, "\x4C\x8B\xC9\x85\xD2\x7E\x00\x8D\x42\xFF\x48\x63\xD0\x48\x8B\x41", "xxxxxx?xxxxxxxxx", 0, lua_State*, int)
	//CREATE_NORMAL_CALLABLE_SIGNATURE(lua_replace, void, "\x56\x57\x8B\x7C\x24\x10\x81\xFF\xEE\xD8\xFF\xFF\x75\x16\x8B\x4C\x24\x0C\x5F\x8B\x41\x14\x8D\x71\x14\x8B\x40\xF8\x83\x06\xF8\x89", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, int)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_remove, void, "\x4C\x8B\xC1\x85\xD2\x7E\x00\x8D\x42\xFF\x48\x63\xD0\x48\x8B\x41\x20\x48\x8B\x49\x28\x48\x8D\x04\xD0\x48\x3B\xC1", "xxxxxx?xxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, int)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_newstate, lua_State*, "\x48\x89\x5C\x24\x08\x48\x89\x6C\x24\x10\x48\x89\x74\x24\x18\x57\x48\x83\xEC\x20\x48\x8B\xF2\x48\x8B\xE9\x48\x8B\xCE\x41\xB9\x70", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_Alloc, void*)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_close, void, "\x48\x89\x5C\x24\x08\x48\x89\x6C\x24\x10\x48\x89\x74\x24\x18\x57\x48\x83\xEC\x20\x48\x8B\x79\x10\x48\x8B\xB7\xC8\x00\x00\x00\x48", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*)

	//CREATE_CALLABLE_SIGNATURE(lua_rawset, void, "\x51\x53\x55\x56\x57\x8B\xF1\xE8\x00\x00\x00\x00", "xxxxxxxx????", 0, lua_State*, int)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_settable, void, "\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\xE8\x00\x00\x00\x00\x4C\x8B\x43\x28\x48\x8B\xD0\x49\x83\xE8\x10", "xxxxxxxxxx????xxxxxxxxxxx", 0, lua_State*, int)

	//CREATE_CALLABLE_SIGNATURE(lua_pushnumber, void, "\x8B\x44\x24\x04\x8B\x48\x08\xF3\x0F\x10\x44\x24\x08", "xxxxxxxxxxxxx", 0, lua_State*, double)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushinteger, void, "\x48\x8B\x41\x28\x0F\x57\xC0\xF2\x48\x0F\x2A\xC2\xF2\x0F\x11\x00", "xxxxxxxxxxxxxxxx", 0, lua_State*, ptrdiff_t)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushboolean, void, "\x48\x8B\x41\x28\x45\x33\xC0\x85\xD2\x41\x0F\x95\xC0\x49\xFF\xC0", "xxxxxxxxxxxxxxxx", 0, lua_State*, bool)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushcclosure, void, "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x4C\x8B\x49\x10\x48\x8B\xF2\x49\x63\xF8\x48\x8B\xD9\x49\x8B\x41\x28", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, lua_CFunction, int)
	// lua_pushstring()'s signature was found before lua_pushlstring()'s, so I'm leaving it here now since it's valid anyway
	// It was used as a quick and dirty - and broken - workaround since most lua_pushlstring() calls are inlined, but it ended up
	// breaking HTTP downloads of zip archives due to its sensitivity to premature null characters
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushlstring, void, "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x4C\x8B\x49\x10\x49\x8B\xF8\x48\x8B\xF2\x48\x8B\xD9\x49\x8B\x41\x28", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, const char*, size_t)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_pushstring, void, "\x48\x89\x5C\x24\x08\x57\x48\x83\xEC\x20\x48\x8B\xFA\x48\x8B\xD9\x48\x85\xD2\x75\x00\x48\x8B\x41\x28\x49\x83\xC8\xFF\x4C\x89\x00", "xxxxxxxxxxxxxxxxxxxx?xxxxxxxxxxx", 0, lua_State*, const char*)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_checkstack, int, "\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\x81\xFA\x40\x1F\x00\x00\x7F", "xxxxxxxxxxxxxxxx", 0, lua_State*, int)

	// luaI_openlib() is really luaL_openlib(), see lauxlib.h in Lua 5.1's source code
	CREATE_NORMAL_CALLABLE_SIGNATURE(luaI_openlib, void, "\x48\x89\x5C\x24\x10\x48\x89\x6C\x24\x18\x48\x89\x74\x24\x20\x57\x48\x83\xEC\x20\x48\x8B\x41\x28", "xxxxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, const char*, const luaL_Reg*, int)
	CREATE_NORMAL_CALLABLE_SIGNATURE(luaL_ref, int, "\x48\x89\x5C\x24\x20\x57\x48\x83\xEC\x20\x8D\x82\x0F\x27\x00\x00", "xxxxxxxxxxxxxxxx", 0, lua_State*, int)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_rawgeti, void, "\x40\x53\x48\x83\xEC\x20\x4D\x63\xD8\x48\x8B\xD9\xE8\x00\x00\x00\x00\x48\xBA\xFF\xFF\xFF\xFF\xFF", "xxxxxxxxxxxxx????xxxxxxx", 0, lua_State*, int, int)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_rawseti, void, "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x4D\x63\xD8\x48\x8B\xD9\xE8", "xxxxxxxxxxxxxxxxxxxxxx", 0, lua_State*, int, int)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_type, int, "\x48\x83\xEC\x28\x4C\x8B\xD9\xE8\xB4\xD9\xFE\xFF\x48\x8B\xD0\x48", "xxxxxxxxxxxxxxxx", 0, lua_State*, int)
	CREATE_NORMAL_CALLABLE_SIGNATURE(lua_typename, const char*, "\x8D\x42\x01\x48\x98\x48\x8D\x0D\xA4\x69\x37\x00\x48\x8B\x04\xC1", "xxxxxxxxxxxxxxx", 0, lua_State*, int)
	CREATE_NORMAL_CALLABLE_SIGNATURE(luaL_unref, void, "\x45\x85\xC0\x0F\x88\x00\x00\x00\x00\x48\x89\x5C\x24\x08\x48\x89", "xxxxx????xxxxxxx", 0, lua_State*, int, int)
	// Replacing the now-extensively-inlined do_game_update() hook with one applied on Application::update() instead
	CREATE_CALLABLE_CLASS_SIGNATURE(application_update, void*, "\x48\x89\x5C\x24\x18\x48\x89\x6C\x24\x20\x56\x57\x41\x56\x48\x83\xEC\x60\x0F\x29\x74\x24\x50\x0F\x29\x7C\x24\x40\x48\x8B\xF9\x65", "xxxxxxxxxxxxxxxxxxxx", 0, long long)
	// Possibly dsl::LuaInterface::newstate() rather than luaL_newstate()
	CREATE_CALLABLE_CLASS_SIGNATURE(luaL_newstate, int, "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x20\x41\x0F\xB6\xF8\x0F\xB6\xF2\x48\x8B", "xxxxxxxxxxxxxxxxxxxxxxxx", 0, char, char, int)

// lua c-functions

// From src/lua.h
// Pseudo-indices
#define LUA_REGISTRYINDEX	(-10000)
#define LUA_ENVIRONINDEX	(-10001)
#define LUA_GLOBALSINDEX	(-10002)
#define lua_upvalueindex(i)	(LUA_GLOBALSINDEX-(i))

// From src/lauxlib.h
#define LUA_NOREF       (-2)
#define LUA_REFNIL      (-1)

// more bloody lua shit
// Thread status; 0 is OK
#define LUA_YIELD	1
#define LUA_ERRRUN	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRERR	5
// From src/lauxlib.h
// Extra error code for 'luaL_load'
#define LUA_ERRFILE     (LUA_ERRERR+1)

// From src/lua.h
// Option for multiple returns in 'lua_pcall' and 'lua_call'
#define LUA_MULTRET	(-1)
#define LUA_TNONE		(-1)
#define LUA_TNIL		0
#define LUA_TBOOLEAN		1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION		6
#define LUA_TUSERDATA		7
#define LUA_TTHREAD		8

#define lua_pop(L,n)		lua_settop(L, -(n)-1)
#define lua_isfunction(L,n)	(lua_type(L, (n)) == LUA_TFUNCTION)
#define lua_istable(L,n)	(lua_type(L, (n)) == LUA_TTABLE)
#define lua_islightuserdata(L,n)	(lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
#define lua_isnil(L,n)		(lua_type(L, (n)) == LUA_TNIL)
#define lua_isboolean(L,n)	(lua_type(L, (n)) == LUA_TBOOLEAN)
#define lua_isthread(L,n)	(lua_type(L, (n)) == LUA_TTHREAD)
#define lua_isnone(L,n)		(lua_type(L, (n)) == LUA_TNONE)
#define lua_isnoneornil(L, n)	(lua_type(L, (n)) <= 0)
#define lua_getglobal(L,s)	lua_getfield(L, LUA_GLOBALSINDEX, (s))
#define lua_setglobal(L,s)	lua_setfield(L, LUA_GLOBALSINDEX, (s))
#define lua_tostring(L,i)	lua_tolstring(L, (i), NULL)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                         BLT Code                                                          //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::list<lua_State*> activeStates;
	void add_active_state(lua_State* L)
	{
		activeStates.push_back(L);
	}

	void remove_active_state(lua_State* L)
	{
		activeStates.remove(L);
	}

	bool check_active_state(lua_State* L)
	{
		std::list<lua_State*>::iterator it;
		for (it = activeStates.begin(); it != activeStates.end(); it++)
		{
			if (*it == L)
			{
				return true;
			}
		}
		return false;
	}

	FuncDetour* luaCallDetour = nullptr;

	// Not tracking the error count here so it can automatically be reset to 0 whenever the Lua state is deleted and re-created (e.g.
	// when transitioning to / from the menu to a level)
	void NotifyErrorOverlay(lua_State* L, const char* message)
	{
		lua_getglobal(L, "NotifyErrorOverlay");
		if (lua_isfunction(L, -1))
		{
			int args = 0;
			if (message)
			{
				lua_pushstring(L, message);
				args = 1;
			}
			int error = lua_pcall(L, args, 0, 0);
			if (error == LUA_ERRRUN)
			{
				// Don't bother logging the error since the error overlay is designed to be an optional component, just pop the error
				// message off the stack to keep it balanced
				lua_pop(L, 1);
				return;
			}
		}
		else
		{
			lua_pop(L, 1);
			static bool printed = false;
			if (!printed)
			{
				printf("Warning: Failed to find the NotifyErrorOverlay function in the Lua environment; no in-game notifications will be displayed for caught errors\n");
				printed = true;
			}
		}
	}

	void lua_newcall(lua_State* L, int args, int returns) {
		// TODO: Optimize this to avoid incurring global table lookup costs whenever a function is called
		// https://stackoverflow.com/questions/30021904/lua-set-default-error-handler/30022216#30022216
		lua_getglobal(L, "debug");
		lua_getfield(L, -1, "traceback");
		lua_remove(L, -2);
		// Do not index from the top (i.e. use a negative index) as this has the potential to mess up if the callee function returns
		// values /and/ lua_pcall() is set up with a > 0 nresults argument
		int errorhandler = lua_gettop(L) - args - 1;
		lua_insert(L, errorhandler);

		int result = lua_pcall(L, args, returns, errorhandler);
		if (result != 0) {
			size_t len;
			const char* message = lua_tolstring(L, -1, &len);
			PD2HOOK_LOG_ERROR(message);
			NotifyErrorOverlay(L, message);
			// This call pops the error message off the stack
			lua_pop(L, 1);
			// To emphasize that the string is no longer on the stack and should not be accessed
			message = nullptr;
		}
		// This call removes the error handler from the stack. Do not use lua_pop() as the callee function's return values may be
		// present, which would pop one of those instead and leave the error handler on the stack
		lua_remove(L, errorhandler);
	}

	int luaF_ispcallforced(lua_State* L)
	{
		lua_pushboolean(L, luaCallDetour ? true : false);
		return 1;
	}

	int luaF_forcepcalls(lua_State* L) {
		int n = lua_gettop(L);	// Number of arguments
		if (n < 1) {
			PD2HOOK_LOG_WARN("blt.forcepcalls(): Called with no arguments, ignoring request");
			return 0;
		}

		if (lua_toboolean(L, 1)) {
			if (!luaCallDetour) {
				luaCallDetour = new FuncDetour(lua_call, lua_newcall);
				lua_call = reinterpret_cast<lua_callptr>(luaCallDetour->GetTrampoline());
				PD2HOOK_LOG_LOG("blt.forcepcalls(): Protected calls will now be forced");
			}
			//		else Logging::Log("blt.forcepcalls(): Protected calls are already being forced, ignoring request", Logging::LOGGING_WARN);
		}
		else {
			if (luaCallDetour) {
				delete luaCallDetour;
				luaCallDetour = nullptr;
				PD2HOOK_LOG_LOG("blt.forcepcalls(): Protected calls are no longer being forced");
			}
			//		else Logging::Log("blt.forcepcalls(): Protected calls are not currently being forced, ignoring request", Logging::LOGGING_WARN);
		}
		return 0;
	}
	int luaH_getcontents(lua_State* L, bool files)
	{
		size_t len;
		const char* dirc = lua_tolstring(L, 1, &len);
		std::string dir(dirc, len);
		std::vector<std::string> directories;

		try
		{
			directories = Util::GetDirectoryContents(dir, files);
		}
		catch (...)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		lua_createtable(L, 0, 0);

		std::vector<std::string>::iterator it;
		int index = 1;
		for (it = directories.begin(); it < directories.end(); it++)
		{
			if (*it == "." || *it == "..") continue;
			lua_pushinteger(L, index);
			lua_pushlstring(L, it->c_str(), it->length());
			lua_settable(L, -3);
			index++;
		}

		return 1;
	}

	int luaF_getdir(lua_State* L)
	{
		return luaH_getcontents(L, true);
	}

	int luaF_getfiles(lua_State* L)
	{
		return luaH_getcontents(L, false);
	}

	int luaF_directoryExists(lua_State* L)
	{
		size_t len;
		const char* dirc = lua_tolstring(L, 1, &len);
		bool doesExist = Util::DirectoryExists(dirc);
		lua_pushboolean(L, doesExist);
		return 1;
	}

	int luaF_unzipfile(lua_State* L)
	{
		size_t len;
		const char* archivePath = lua_tolstring(L, 1, &len);
		const char* extractPath = lua_tolstring(L, 2, &len);

		pd2hook::ExtractZIPArchive(archivePath, extractPath);
		return 0;
	}

	int luaF_removeDirectory(lua_State* L)
	{
		size_t len;
		const char* directory = lua_tolstring(L, 1, &len);
		bool success = Util::RemoveEmptyDirectory(directory);
		lua_pushboolean(L, success);
		return 1;
	}

	int luaF_pcall(lua_State* L)
	{
		int args = lua_gettop(L) - 1;

		lua_getglobal(L, "debug");
		lua_getfield(L, -1, "traceback");
		lua_remove(L, -2);
		// Do not index from the top (i.e. don't use a negative index)
		int errorhandler = lua_gettop(L) - args - 1;
		lua_insert(L, errorhandler);

		int result = lua_pcall(L, args, LUA_MULTRET, errorhandler);
		// lua_pcall() automatically pops the callee function and its arguments off the stack. Then, if no errors were encountered
		// during execution, it pushes the return values onto the stack, if any. Otherwise, if an error was encountered, it pushes
		// the error message onto the stack, which should manually be popped off when done using to keep the stack balanced
		if (result == LUA_ERRRUN)
		{
			size_t len;
			PD2HOOK_LOG_ERROR(lua_tolstring(L, -1, &len));
			// This call pops the error message off the stack
			lua_pop(L, 1);
			return 0;
		}
		// Do not use lua_pop() as the callee function's return values may be present, which would pop one of those instead and leave
		// the error handler on the stack
		lua_remove(L, errorhandler);
		lua_pushboolean(L, result == 0);
		lua_insert(L, 1);

		//if (result != 0) return 1;

		return lua_gettop(L);
	}

	int luaF_dofile(lua_State* L)
	{
		int n = lua_gettop(L);

		size_t length = 0;
		const char* filename = lua_tolstring(L, 1, &length);
		int error = luaL_loadfilex(L, filename, nullptr);
		if (error != 0)
		{
			size_t len;
			//		Logging::Log(filename, Logging::LOGGING_ERROR);
			PD2HOOK_LOG_ERROR(lua_tolstring(L, -1, &len));
		}
		else
		{
			lua_getglobal(L, "debug");
			lua_getfield(L, -1, "traceback");
			lua_remove(L, -2);
			// Example stack for visualization purposes:
			// 3 debug.traceback
			// 2 compiled code as a self-contained function
			// 1 filename
			// Do not index from the top (i.e. don't use a negative index)
			int errorhandler = 2;
			lua_insert(L, errorhandler);

			error = lua_pcall(L, 0, 0, errorhandler);
			if (error == LUA_ERRRUN)
			{
				size_t len;
				//			Logging::Log(filename, Logging::LOGGING_ERROR);
				PD2HOOK_LOG_ERROR(lua_tolstring(L, -1, &len));
				// This call pops the error message off the stack
				lua_pop(L, 1);
			}
			// Do not use lua_pop() as the callee function's return values may be present, which would pop one of those instead and
			// leave the error handler on the stack
			lua_remove(L, errorhandler);
		}
		return 0;
	}

	struct lua_http_data
	{
		int funcRef;
		int progressRef;
		int requestIdentifier;
		lua_State* L;
	};

	void return_lua_http(void* data, std::string& urlcontents)
	{
		lua_http_data* ourData = (lua_http_data*)data;
		if (!check_active_state(ourData->L))
		{
			delete ourData;
			return;
		}

		lua_rawgeti(ourData->L, LUA_REGISTRYINDEX, ourData->funcRef);
		lua_pushlstring(ourData->L, urlcontents.c_str(), urlcontents.length());
		lua_pushinteger(ourData->L, ourData->requestIdentifier);
		lua_pcall(ourData->L, 2, 0, 0);
		luaL_unref(ourData->L, LUA_REGISTRYINDEX, ourData->funcRef);
		luaL_unref(ourData->L, LUA_REGISTRYINDEX, ourData->progressRef);
		delete ourData;
	}

	void progress_lua_http(void* data, long progress, long total)
	{
		lua_http_data* ourData = (lua_http_data*)data;

		if (!check_active_state(ourData->L))
		{
			return;
		}

		if (ourData->progressRef == 0) return;
		lua_rawgeti(ourData->L, LUA_REGISTRYINDEX, ourData->progressRef);
		lua_pushinteger(ourData->L, ourData->requestIdentifier);
		lua_pushinteger(ourData->L, progress);
		lua_pushinteger(ourData->L, total);
		lua_pcall(ourData->L, 3, 0, 0);
	}

	int luaF_directoryhash(lua_State* L)
	{
		PD2HOOK_TRACE_FUNC;
		int n = lua_gettop(L);

		size_t length = 0;
		const char* filename = lua_tolstring(L, 1, &length);
		std::string hash = Util::GetDirectoryHash(filename);
		lua_pushlstring(L, hash.c_str(), hash.length());

		return 1;
	}

	int luaF_filehash(lua_State* L)
	{
		int n = lua_gettop(L);
		size_t l = 0;
		const char * fileName = lua_tolstring(L, 1, &l);
		std::string hash = Util::GetFileHash(fileName);
		lua_pushlstring(L, hash.c_str(), hash.length());
		return 1;
	}

	static int HTTPReqIdent = 0;

	int luaF_dohttpreq(lua_State* L)
	{
		PD2HOOK_LOG_LOG("Incoming HTTP Request/Request");

		int args = lua_gettop(L);
		int progressReference = 0;
		if (args >= 3)
		{
			progressReference = luaL_ref(L, LUA_REGISTRYINDEX);
		}

		int functionReference = luaL_ref(L, LUA_REGISTRYINDEX);
		size_t len;
		const char* url_c = lua_tolstring(L, 1, &len);
		std::string url = std::string(url_c, len);

		PD2HOOK_LOG_LOG(std::string(url_c, len) << " - " << functionReference);

		lua_http_data* ourData = new lua_http_data();
		ourData->funcRef = functionReference;
		ourData->progressRef = progressReference;
		ourData->L = L;

		HTTPReqIdent++;
		ourData->requestIdentifier = HTTPReqIdent;

		std::unique_ptr<HTTPItem> reqItem(new HTTPItem());
		reqItem->call = return_lua_http;
		reqItem->data = ourData;
		reqItem->url = url;

		if (progressReference != 0)
		{
			reqItem->progress = progress_lua_http;
		}

		HTTPManager::GetSingleton()->LaunchHTTPRequest(std::move(reqItem));
		lua_pushinteger(L, HTTPReqIdent);
		return 1;
	}

	CConsole* gbl_mConsole = NULL;

	int luaF_createconsole(lua_State* L)
	{
		if (gbl_mConsole) return 0;
		gbl_mConsole = new CConsole();
		return 0;
	}

	int luaF_destroyconsole(lua_State* L)
	{
		if (!gbl_mConsole) return 0;
		delete gbl_mConsole;
		gbl_mConsole = NULL;
		return 0;
	}

	int luaF_print(lua_State* L)
	{
		int top = lua_gettop(L);
		std::stringstream stream;
		for (int i = 0; i < top; ++i)
		{
			size_t len;
			const char* str = lua_tolstring(L, i + 1, &len);
			stream << (i > 0 ? "    " : "") << str;
		}
		PD2HOOK_LOG_LUA(stream.str());

		return 0;
	}

	int luaF_moveDirectory(lua_State * L)
	{
		int top = lua_gettop(L);
		size_t lf = 0;
		const char * fromStr = lua_tolstring(L, 1, &lf);
		size_t ld = 0;
		const char * destStr = lua_tolstring(L, 2, &ld);

		bool success = Util::MoveDirectory(fromStr, destStr);
		lua_pushboolean(L, success);
		return 1;
	}

	int updates = 0;
	std::thread::id main_thread_id;

	void* application_update_new(void* thisptr, long long llUnk0)
	{
		// If someone has a better way of doing this, I'd like to know about it.
		// I could save the this pointer?
		// I'll check if it's even different at all later.
		if (std::this_thread::get_id() != main_thread_id)
		{
			return application_update(thisptr, llUnk0);
		}

		if (updates == 0)
		{
			HTTPManager::GetSingleton()->init_locks();
		}

		if (updates > 1)
		{
			EventQueueMaster::GetSingleton().ProcessEvents();
		}

		updates++;
		return application_update(thisptr, llUnk0);
	}

	// Random dude who wrote what's his face?
	// I 'unno, I stole this method from the guy who wrote the 'underground-light-lua-hook'
	// Mine worked fine, but this seems more elegant.
	void* luaL_newstate_new(void* thislol, bool no, bool freakin, int clue)
	{
		void* ret = reinterpret_cast<void*>(luaL_newstate(thislol, no, freakin, clue));

		lua_State* L = (lua_State*)*((void**)thislol);
		printf("Lua State: %p\n", (void*)L);
		if (!L) return ret;

		add_active_state(L);

		lua_pushcclosure(L, luaF_print, 0);
		lua_setfield(L, LUA_GLOBALSINDEX, "log");

		lua_pushcclosure(L, luaF_pcall, 0);
		lua_setfield(L, LUA_GLOBALSINDEX, "pcall");

		lua_pushcclosure(L, luaF_dofile, 0);
		lua_setfield(L, LUA_GLOBALSINDEX, "dofile");

		lua_pushcclosure(L, luaF_unzipfile, 0);
		lua_setfield(L, LUA_GLOBALSINDEX, "unzip");

		lua_pushcclosure(L, luaF_dohttpreq, 0);
		lua_setfield(L, LUA_GLOBALSINDEX, "dohttpreq");

		luaL_Reg consoleLib[] = {
			{ "CreateConsole", luaF_createconsole },
			{ "DestroyConsole", luaF_destroyconsole },
			{ NULL, NULL }
		};
		luaI_openlib(L, "console", consoleLib, 0);

		luaL_Reg fileLib[] = {
			{ "GetDirectories", luaF_getdir },
			{ "GetFiles", luaF_getfiles },
			{ "RemoveDirectory", luaF_removeDirectory },
			{ "DirectoryExists", luaF_directoryExists },
			{ "DirectoryHash", luaF_directoryhash },
			{ "FileHash", luaF_filehash },
			{ "MoveDirectory", luaF_moveDirectory },
			{ NULL, NULL }
		};
		luaI_openlib(L, "file", fileLib, 0);

		// Keeping everything in lowercase since IspcallForced / IsPCallForced and Forcepcalls / ForcePCalls look rather weird anyway
		luaL_Reg bltLib[] = {
			{ "ispcallforced", luaF_ispcallforced },
			{ "forcepcalls", luaF_forcepcalls },
			{ NULL, NULL }
		};
		luaI_openlib(L, "blt", bltLib, 0);

		int result;
		PD2HOOK_LOG_LOG("Initiating Hook");

		result = luaL_loadfilex(L, "mods/base/base.lua", nullptr);
		if (result == LUA_ERRSYNTAX)
		{
			size_t len;
			PD2HOOK_LOG_ERROR(lua_tolstring(L, -1, &len));
			return ret;
		}
		result = lua_pcall(L, 0, 1, 0);
		if (result == LUA_ERRRUN)
		{
			size_t len;
			PD2HOOK_LOG_LOG(lua_tolstring(L, -1, &len));
			return ret;
		}

		return ret;
	}

	void luaF_close(lua_State* L)
	{
		remove_active_state(L);
		lua_close(L);
	}

	FuncDetour* gameUpdateDetour = nullptr;
	FuncDetour* newStateDetour = nullptr;
	FuncDetour* luaCloseDetour = nullptr;

	void InitiateStates() {
		main_thread_id = std::this_thread::get_id();

		if (!InitializeMinHook()) {
			printf("Critical Error: MinHook library failed to initialize\n");
		}

		SignatureSearch::Search();

		gameUpdateDetour = new FuncDetour(application_update, application_update_new);
		application_update = reinterpret_cast<application_updateptr>(gameUpdateDetour->GetTrampoline());

		newStateDetour = new FuncDetour(luaL_newstate, luaL_newstate_new);
		luaL_newstate = reinterpret_cast<luaL_newstateptr>(newStateDetour->GetTrampoline());

		//	luaCallDetour = new FuncDetour(lua_call, lua_newcall);
		//	lua_call = reinterpret_cast<lua_callptr>(luaCallDetour->GetTrampoline());

		luaCloseDetour = new FuncDetour(lua_close, luaF_close);
		lua_close = reinterpret_cast<lua_closeptr>(luaCloseDetour->GetTrampoline());

		std::ifstream infile("mods/developer.txt");
		if (infile.good())
		{
			gbl_mConsole = new CConsole();
		}
	}

	void DestroyStates() {
		// Okay... let's not do that.
		// I don't want to keep this in memory, but it CRASHES THE SHIT OUT if you delete this after all is said and done.
		// if (gbl_mConsole) delete gbl_mConsole;

		if (gameUpdateDetour) delete gameUpdateDetour;
		if (newStateDetour) delete newStateDetour;
		if (luaCallDetour) delete luaCallDetour;
		if (luaCloseDetour) delete luaCloseDetour;
		if (gbl_mConsole) delete gbl_mConsole;

		UninitializeMinHook();

		HTTPManager::Destroy(); // exit crash fix
		Logging::Logger::Close(); // exit crash fix
	}
}