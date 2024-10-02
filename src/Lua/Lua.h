#pragma once

#include "FunctionHook.h"
#include <memory>

extern "C" {
#include <Lua/luajit/luajit/src/lua.h>
#include <Lua/luajit/luajit/src/lauxlib.h>
}

#define DECLARE_LUA_PATTERN_FUNC(name, ret, ...) \
	extern ret(*name)(__VA_ARGS__);

DECLARE_LUA_PATTERN_FUNC(lua_call_exe, void, lua_State*, int, int)
DECLARE_LUA_PATTERN_FUNC(lua_close_exe, void, lua_State*)
DECLARE_LUA_PATTERN_FUNC(ctor_lua_State_exe, void*, void*, void*, size_t, size_t)

void lua_init(void(*luaFuncReg)(lua_State* L));

bool check_active_state(lua_State* L);

void UpdateStates();

FunctionHook<void, lua_State*, int, int>& GetNewCallFunctionHook();
