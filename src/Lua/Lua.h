#ifndef __LUA_H__
#define __LUA_H__

#include "FunctionHook.h"
#include <memory>

struct lua_State;

typedef const char* (*lua_Reader)(lua_State* L, void* ud, size_t* sz);
typedef int (*lua_CFunction)(lua_State* L);
typedef void* (*lua_Alloc)(void* ud, void* ptr, size_t osize, size_t nsize);
typedef struct luaL_Reg
{
	const char* name;
	lua_CFunction func;
} luaL_Reg;


// From src/luaconf.h
#define LUA_NUMBER double

	// From src/lua.h
	// type of numbers in Lua
typedef LUA_NUMBER lua_Number;
typedef struct lua_Debug lua_Debug; // activation record
// Functions to be called by the debuger in specific events
typedef void (*lua_Hook)(lua_State* L, lua_Debug* ar);

// From src/luaconf.h
#define LUA_NUMBER double

	// From src/lua.h
	// type of numbers in Lua
typedef LUA_NUMBER lua_Number;
typedef struct lua_Debug lua_Debug; // activation record
// Functions to be called by the debuger in specific events
typedef void (*lua_Hook)(lua_State* L, lua_Debug* ar);


// From src/lua.h
// Pseudo-indices
#define LUA_REGISTRYINDEX (-10000)
#define LUA_ENVIRONINDEX (-10001)
#define LUA_GLOBALSINDEX (-10002)
#define lua_upvalueindex(i) (LUA_GLOBALSINDEX - (i))

// From src/lauxlib.h
#define LUA_NOREF (-2)
#define LUA_REFNIL (-1)

// more bloody lua shit
// Thread status; 0 is OK
#define LUA_YIELD 1
#define LUA_ERRRUN 2
#define LUA_ERRSYNTAX 3
#define LUA_ERRMEM 4
#define LUA_ERRERR 5
// From src/lauxlib.h
// Extra error code for 'luaL_load'
#define LUA_ERRFILE (LUA_ERRERR + 1)

// From src/lua.h
// Option for multiple returns in 'lua_pcall' and 'lua_call'
#define LUA_MULTRET (-1)
#define LUA_TNONE (-1)
#define LUA_TNIL 0
#define LUA_TBOOLEAN 1
#define LUA_TLIGHTUSERDATA 2
#define LUA_TNUMBER 3
#define LUA_TSTRING 4
#define LUA_TTABLE 5
#define LUA_TFUNCTION 6
#define LUA_TUSERDATA 7
#define LUA_TTHREAD 8

// lua c-functions

#define DECLARE_LUA_PATTERN_FUNC(name, ret, ...) \
	extern ret(*name)(__VA_ARGS__);

DECLARE_LUA_PATTERN_FUNC(lua_call, void, lua_State*, int, int)
DECLARE_LUA_PATTERN_FUNC(lua_pcall, int, lua_State*, int, int, int)
DECLARE_LUA_PATTERN_FUNC(lua_gettop, int, lua_State*)
DECLARE_LUA_PATTERN_FUNC(lua_settop, void, lua_State*, int)
DECLARE_LUA_PATTERN_FUNC(lua_toboolean, int, lua_State*, int)
DECLARE_LUA_PATTERN_FUNC(lua_tointeger, ptrdiff_t, lua_State*, int)
DECLARE_LUA_PATTERN_FUNC(lua_tonumber, lua_Number, lua_State*, int)
DECLARE_LUA_PATTERN_FUNC(lua_tolstring, const char*, lua_State*, int, size_t*)
DECLARE_LUA_PATTERN_FUNC(lua_objlen, size_t, lua_State*, int)
DECLARE_LUA_PATTERN_FUNC(luaL_loadfilex, int, lua_State*, const char*, const char*)
DECLARE_LUA_PATTERN_FUNC(luaL_loadstring, int, lua_State*, const char*)
DECLARE_LUA_PATTERN_FUNC(lua_getfield, void, lua_State*, int, const char*)
DECLARE_LUA_PATTERN_FUNC(lua_setfield, int, lua_State*, int, const char*)
DECLARE_LUA_PATTERN_FUNC(lua_createtable, void, lua_State*, int, int)
DECLARE_LUA_PATTERN_FUNC(lua_insert, void, lua_State*, int)
DECLARE_LUA_PATTERN_FUNC(lua_remove, void, lua_State*, int)
DECLARE_LUA_PATTERN_FUNC(lua_newstate, lua_State*, lua_Alloc, void*)
DECLARE_LUA_PATTERN_FUNC(lua_close, void, lua_State*)
DECLARE_LUA_PATTERN_FUNC(lua_settable, void, lua_State*, int)
DECLARE_LUA_PATTERN_FUNC(lua_pushinteger, void, lua_State*, ptrdiff_t)
DECLARE_LUA_PATTERN_FUNC(lua_pushboolean, void, lua_State*, bool)
DECLARE_LUA_PATTERN_FUNC(lua_pushcclosure, void, lua_State*, lua_CFunction, int)
DECLARE_LUA_PATTERN_FUNC(lua_pushlstring, void, lua_State*, const char*, size_t)
DECLARE_LUA_PATTERN_FUNC(lua_pushstring, void, lua_State*, const char*)
DECLARE_LUA_PATTERN_FUNC(lua_checkstack, int, lua_State*, int)
DECLARE_LUA_PATTERN_FUNC(luaI_openlib, void, lua_State*, const char*, const luaL_Reg*, int)
DECLARE_LUA_PATTERN_FUNC(luaL_ref, int, lua_State*, int)
DECLARE_LUA_PATTERN_FUNC(lua_rawgeti, void, lua_State*, int, int)
DECLARE_LUA_PATTERN_FUNC(lua_rawseti, void, lua_State*, int, int)
DECLARE_LUA_PATTERN_FUNC(lua_type, int, lua_State*, int)
//DECLARE_LUA_PATTERN_FUNC(lua_typename, const char*, lua_State*, int)
DECLARE_LUA_PATTERN_FUNC(luaL_unref, void, lua_State*, int, int)
DECLARE_LUA_PATTERN_FUNC(luaL_newstate, void*, void*, bool, bool, int)

#undef DECLARE_LUA_PATTERN_FUNC

#define lua_pop(L, n) lua_settop(L, -(n)-1)
#define lua_isfunction(L, n) (lua_type(L, (n)) == LUA_TFUNCTION)
#define lua_istable(L, n) (lua_type(L, (n)) == LUA_TTABLE)
#define lua_islightuserdata(L, n) (lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
#define lua_isnil(L, n) (lua_type(L, (n)) == LUA_TNIL)
#define lua_isboolean(L, n) (lua_type(L, (n)) == LUA_TBOOLEAN)
#define lua_isthread(L, n) (lua_type(L, (n)) == LUA_TTHREAD)
#define lua_isnone(L, n) (lua_type(L, (n)) == LUA_TNONE)
#define lua_isnoneornil(L, n) (lua_type(L, (n)) <= 0)
#define lua_getglobal(L, s) lua_getfield(L, LUA_GLOBALSINDEX, (s))
#define lua_setglobal(L, s) lua_setfield(L, LUA_GLOBALSINDEX, (s))
#define lua_tostring(L, i) lua_tolstring(L, (i), NULL)

void lua_init(void(*luaFuncReg)(lua_State* L));

bool check_active_state(lua_State* L);

FunctionHook<void, lua_State*, int, int>& GetNewCallFunctionHook();

#endif
