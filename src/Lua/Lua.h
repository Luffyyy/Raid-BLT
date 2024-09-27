#ifndef __LUA_H__
#define __LUA_H__

#include "FunctionHook.h"
#include <memory>

extern "C" {
    #include <Lua/luajit/luajit/src/lua.h>
    #include <Lua/luajit/luajit/src/lauxlib.h>
}

#define DECLARE_LUA_PATTERN_FUNC(name, ret, ...) \
	extern ret(*name)(__VA_ARGS__);

DECLARE_LUA_PATTERN_FUNC(luaL_newstate_exe, void*, void*)

void lua_init(void(*luaFuncReg)(lua_State* L));

bool check_active_state(lua_State* L);

FunctionHook<void, lua_State*, int, int>& GetNewCallFunctionHook();

#endif
