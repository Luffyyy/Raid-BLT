cmake_minimum_required (VERSION 3.15)

add_subdirectory(Lua/luajit)

set(IPHLPAPI_SOURCES
    SearchPattern.cpp
	SearchPattern.h
	SearchRange.cpp
	SearchRange.h
	StringConstants.h
	Disassembler.cpp
	Disassembler.h
	FunctionHook.cpp
	FunctionHook.h
	FunctionPattern.h
	Game.cpp
	Game.h
	InitiateState.cpp
	InitState.h
	iphp.cpp
	Logger.cpp
	Logger.h
    console/console.h
    console/console.cpp
    http/http.h
    http/http.cpp
	Lua/CustomLua.cpp
	Lua/CustomLua.h
	Lua/Lua.cpp
	Lua/Lua.h
	Lua/LuaApplication.cpp
	Lua/LuaApplication.hpp
    threading/queue.h
    threading/threadqueue.cpp
    util/compression.cpp
    util/files.cpp
    util/util.cpp
    util/util.h
	plugins/plugins.h
	plugins/plugins.cpp
	../Resource.h
	../Resource.rc
)

add_library(IPHLPAPI SHARED ${IPHLPAPI_SOURCES})

target_link_libraries(IPHLPAPI PUBLIC Ws2_32 libcryptoMT libsslMT Wldap32 libcurl zlibstatic minhook Zycore Zydis Version lua51-objects
    ${CMAKE_CURRENT_SOURCE_DIR}/Lua/luajit/lj_vm.obj)

target_compile_definitions(IPHLPAPI PUBLIC WIN32_LEAN_AND_MEAN LUA_BUILD_AS_DLL LUA_LIB)

target_include_directories(IPHLPAPI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../lib/openssl-1.1.0f-vs2017/Include64 
                ${CMAKE_CURRENT_SOURCE_DIR}/../lib/minhook/include ${CMAKE_CURRENT_SOURCE_DIR}/../lib/zlib-1.2.11 ${CMAKE_BINARY_DIR}/lib/zlib-1.2.11)

target_link_directories(IPHLPAPI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../lib/openssl-1.1.0f-vs2017/lib)

set_property(TARGET IPHLPAPI PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_property(TARGET IPHLPAPI PROPERTY CXX_STANDARD 20)

set_target_properties(IPHLPAPI PROPERTIES LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/../iphp.def\"")