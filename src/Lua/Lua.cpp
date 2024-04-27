#include "Lua.h"
#include "FunctionHook.h"
#include "FunctionPattern.h"
#include "Logger.h"

#include <list>

template<typename TRet, typename... TArgs>
auto FindFunctionAddress(std::string_view strName, std::string_view strPattern)
{
	TRet(*result)(TArgs...);

	SearchPattern pattern(strPattern);

	result = reinterpret_cast<TRet(*)(TArgs...)>(pattern.Match(
		SearchRange::GetStartSearchAddress(), SearchRange::GetSearchSize()));

	if (result == nullptr)
	{
		PD2HOOK_LOG_ERROR("Function '{}' not found", strName);
	}
	else
	{
		PD2HOOK_LOG_LOG("Function '{0}' found -> 0x{1:016x}", strName, reinterpret_cast<uint64_t>(result));
	}

	return result;
}

void(*s_LuaNewStateCallback)(lua_State* L) = nullptr;

static std::list<lua_State*> activeStates;

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

void lua_newcall(lua_State* L, int args, int returns)
{
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
	if (result != 0)
	{
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

void luaF_close(lua_State* L)
{
	remove_active_state(L);
	lua_close(L);
}

void* luaL_newstate_new(void* thislol, bool no, bool freakin, int clue)
{
	void* ret = reinterpret_cast<void*>(luaL_newstate(thislol, no, freakin, clue));

	lua_State* L = (lua_State*)*((void**)thislol);

	PD2HOOK_LOG_LOG("Lua State: 0x{0:016x}", reinterpret_cast<uint64_t>(L));

	if (!L)
		return ret;

	add_active_state(L);

	if (s_LuaNewStateCallback != nullptr)
		s_LuaNewStateCallback(L);

	return ret;
}

#define LUA_FUNCTION_PATTERN_HOOK(name, target) \
	auto name ## _hook = CreateFunctionHook(#name, target, name ## _pattern);
 
LUA_FUNCTION_PATTERN_HOOK(lua_call, lua_newcall)
LUA_FUNCTION_PATTERN_HOOK(luaL_newstate, luaL_newstate_new)
LUA_FUNCTION_PATTERN_HOOK(lua_close, luaF_close)

#define DEFINE_LUA_PATTERN_FUNC(name, ret, ...) \
	ret(*name)(__VA_ARGS__) = FindFunctionAddress<ret, __VA_ARGS__>(#name, name ## _pattern);

DEFINE_LUA_PATTERN_FUNC(lua_pcall, int, lua_State*, int, int, int)
DEFINE_LUA_PATTERN_FUNC(lua_gettop, int, lua_State*)
DEFINE_LUA_PATTERN_FUNC(lua_settop, void, lua_State*, int)
DEFINE_LUA_PATTERN_FUNC(lua_toboolean, int, lua_State*, int)
DEFINE_LUA_PATTERN_FUNC(lua_tointeger, ptrdiff_t, lua_State*, int)
DEFINE_LUA_PATTERN_FUNC(lua_tonumber, lua_Number, lua_State*, int)
DEFINE_LUA_PATTERN_FUNC(lua_tolstring, const char*, lua_State*, int, size_t*)
DEFINE_LUA_PATTERN_FUNC(lua_objlen, size_t, lua_State*, int)
DEFINE_LUA_PATTERN_FUNC(luaL_loadfilex, int, lua_State*, const char*, const char*)
DEFINE_LUA_PATTERN_FUNC(luaL_loadstring, int, lua_State*, const char*)
DEFINE_LUA_PATTERN_FUNC(lua_getfield, void, lua_State*, int, const char*)
DEFINE_LUA_PATTERN_FUNC(lua_setfield, int, lua_State*, int, const char*)
DEFINE_LUA_PATTERN_FUNC(lua_createtable, void, lua_State*, int, int)
DEFINE_LUA_PATTERN_FUNC(lua_insert, void, lua_State*, int)
DEFINE_LUA_PATTERN_FUNC(lua_remove, void, lua_State*, int)
DEFINE_LUA_PATTERN_FUNC(lua_newstate, lua_State*, lua_Alloc, void*)
DEFINE_LUA_PATTERN_FUNC(lua_settable, void, lua_State*, int)
DEFINE_LUA_PATTERN_FUNC(lua_pushinteger, void, lua_State*, ptrdiff_t)
DEFINE_LUA_PATTERN_FUNC(lua_pushboolean, void, lua_State*, bool)
DEFINE_LUA_PATTERN_FUNC(lua_pushcclosure, void, lua_State*, lua_CFunction, int)
DEFINE_LUA_PATTERN_FUNC(lua_pushlstring, void, lua_State*, const char*, size_t)
DEFINE_LUA_PATTERN_FUNC(lua_pushstring, void, lua_State*, const char*)
DEFINE_LUA_PATTERN_FUNC(lua_checkstack, int, lua_State*, int)
DEFINE_LUA_PATTERN_FUNC(luaI_openlib, void, lua_State*, const char*, const luaL_Reg*, int)
DEFINE_LUA_PATTERN_FUNC(luaL_ref, int, lua_State*, int)
DEFINE_LUA_PATTERN_FUNC(lua_rawgeti, void, lua_State*, int, int)
DEFINE_LUA_PATTERN_FUNC(lua_rawseti, void, lua_State*, int, int)
DEFINE_LUA_PATTERN_FUNC(lua_type, int, lua_State*, int)
DEFINE_LUA_PATTERN_FUNC(lua_typename, const char*, lua_State*, int)
DEFINE_LUA_PATTERN_FUNC(luaL_unref, void, lua_State*, int, int)

void(*lua_call)(lua_State*, int, int) = nullptr;
void* (*luaL_newstate)(void*, bool, bool, int) = nullptr;
void(*lua_close)(lua_State*) = nullptr;

void lua_init(void(*luaFuncReg)(lua_State* L))
{
	lua_call_hook.GetNewFunction(lua_call);
	luaL_newstate_hook.GetNewFunction(luaL_newstate);
	lua_close_hook.GetNewFunction(lua_close);

	s_LuaNewStateCallback = luaFuncReg;
}

FunctionHook<void, lua_State*, int, int>& GetNewCallFunctionHook()
{
	return lua_call_hook;
}
