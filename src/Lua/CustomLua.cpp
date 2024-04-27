#include "CustomLua.h"
#include "LuaApplication.hpp"

#include "util/util.h"
#include "http/http.h"

#include <filesystem>

int luaF_GetDllVersion(lua_State *L)
{
	HMODULE hModule;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)luaF_GetDllVersion, &hModule);
	char path[MAX_PATH + 1];
	size_t pathSize = GetModuleFileName(hModule, path, sizeof(path) - 1);
	path[pathSize] = '\0';

	DWORD verHandle = 0;
	UINT size = 0;
	LPBYTE lpBuffer = NULL;
	uint32_t verSize = GetFileVersionInfoSize(path, &verHandle);

	if (verSize == 0)
	{
		PD2HOOK_LOG_ERROR("Error occurred while calling 'GetFileVersionInfoSize': {}", GetLastError());
		lua_pushstring(L, "0.0.0.0");
		return 1;
	}

	std::string verData;
	verData.resize(verSize);

	if (!GetFileVersionInfo(path, verHandle, verSize, verData.data()))
	{
		PD2HOOK_LOG_ERROR("Error occurred while calling 'GetFileVersionInfo': {}", GetLastError());
		lua_pushstring(L, "0.0.0.0");
		return 1;
	}

	if (!VerQueryValue(verData.data(), "\\", (VOID FAR * FAR *)&lpBuffer, &size))
	{
		PD2HOOK_LOG_ERROR("Error occurred while calling 'VerQueryValue': {}", GetLastError());
		lua_pushstring(L, "0.0.0.0");
		return 1;
	}

	if (size == 0)
	{
		PD2HOOK_LOG_ERROR("Invalid version value buffer Size");
		lua_pushstring(L, "0.0.0.0");
		return 1;
	}

	VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;
	if (verInfo->dwSignature != 0xfeef04bd)
	{
		PD2HOOK_LOG_ERROR("Invalid version signature");
		lua_pushstring(L, "0.0.0.0");
		return 1;
	}

	std::string strVersion = std::format("{}.{}.{}.{}",
										 (verInfo->dwFileVersionMS >> 16) & 0xFFFF,
										 (verInfo->dwFileVersionMS >> 0) & 0xFFFF,
										 (verInfo->dwFileVersionLS >> 16) & 0xFFFF,
										 (verInfo->dwFileVersionLS >> 0) & 0xFFFF);

	lua_pushstring(L, strVersion.c_str());
	return 1;
}

int luaF_EnableApplicationLog(lua_State *L)
{
	int n = lua_gettop(L); // Number of arguments
	if (n < 1)
	{
		PD2HOOK_LOG_WARN("blt.EnableApplicationLog(): Called with no arguments");
		return 0;
	}

	EnableApplicationLog(lua_toboolean(L, 1));
	return 0;
}

int luaF_ispcallforced(lua_State *L)
{
	lua_pushboolean(L, GetNewCallFunctionHook().IsInitialized() ? true : false);
	return 1;
}

int luaF_forcepcalls(lua_State *L)
{
	int n = lua_gettop(L); // Number of arguments
	if (n < 1)
	{
		PD2HOOK_LOG_WARN("blt.forcepcalls(): Called with no arguments, ignoring request");
		return 0;
	}

	auto &lua_call_hook = GetNewCallFunctionHook();

	if (lua_toboolean(L, 1))
	{
		if (!lua_call_hook.IsInitialized())
		{
			lua_call_hook.Initialize();
			PD2HOOK_LOG_LOG("blt.forcepcalls(): Protected calls will now be forced");
		}
		//		else Logging::Log("blt.forcepcalls(): Protected calls are already being forced, ignoring request", Logging::LOGGING_WARN);
	}
	else
	{
		if (lua_call_hook.IsInitialized())
		{
			lua_call_hook.Destroy();
			PD2HOOK_LOG_LOG("blt.forcepcalls(): Protected calls are no longer being forced");
		}
		//		else Logging::Log("blt.forcepcalls(): Protected calls are not currently being forced, ignoring request", Logging::LOGGING_WARN);
	}
	return 0;
}

int luaH_getcontents(lua_State *L, bool files)
{
	size_t len;
	const char *dirc = lua_tolstring(L, 1, &len);
	std::string dir(dirc, len);
	std::vector<std::string> directories;

	try
	{
		directories = pd2hook::Util::GetDirectoryContents(dir, files);
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
		if (*it == "." || *it == "..")
			continue;
		lua_pushinteger(L, index);
		lua_pushlstring(L, it->c_str(), it->length());
		lua_settable(L, -3);
		index++;
	}

	return 1;
}

int luaF_getdir(lua_State *L)
{
	return luaH_getcontents(L, true);
}

int luaF_getfiles(lua_State *L)
{
	return luaH_getcontents(L, false);
}

int luaF_directoryExists(lua_State *L)
{
	size_t len;
	const char *dirc = lua_tolstring(L, 1, &len);
	bool doesExist = pd2hook::Util::DirectoryExists(dirc);
	lua_pushboolean(L, doesExist);
	return 1;
}

int luaF_unzipfile(lua_State *L)
{
	size_t len;
	const char *archivePath = lua_tolstring(L, 1, &len);
	const char *extractPath = lua_tolstring(L, 2, &len);

	pd2hook::ExtractZIPArchive(archivePath, extractPath);
	return 0;
}

int luaF_removeDirectory(lua_State *L)
{
	size_t len;
	const char *directory = lua_tolstring(L, 1, &len);
	bool success = pd2hook::Util::RemoveEmptyDirectory(directory);
	lua_pushboolean(L, success);
	return 1;
}

int luaF_pcall(lua_State *L)
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

	// if (result != 0) return 1;

	return lua_gettop(L);
}

int luaF_dofile(lua_State *L)
{
	int n = lua_gettop(L);

	size_t length = 0;
	const char *filename = lua_tolstring(L, 1, &length);
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
	lua_State *L;
};

void return_lua_http(void *data, std::string &urlcontents)
{
	lua_http_data *ourData = (lua_http_data *)data;
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

void progress_lua_http(void *data, long progress, long total)
{
	lua_http_data *ourData = (lua_http_data *)data;

	if (!check_active_state(ourData->L))
	{
		return;
	}

	if (ourData->progressRef == 0)
		return;
	lua_rawgeti(ourData->L, LUA_REGISTRYINDEX, ourData->progressRef);
	lua_pushinteger(ourData->L, ourData->requestIdentifier);
	lua_pushinteger(ourData->L, progress);
	lua_pushinteger(ourData->L, total);
	lua_pcall(ourData->L, 3, 0, 0);
}

void validate_mod_base()
{
	if (!std::filesystem::exists("mods/base/mod.xml") || !std::filesystem::exists("mods/base/base.lua"))
	{
		PD2HOOK_LOG_LOG("Downloading Mod Base");

		if (!pd2hook::HTTPManager::GetSingleton()->AreLocksInit())
			pd2hook::HTTPManager::GetSingleton()->init_locks();

		pd2hook::HTTPManager::GetSingleton()->DownloadFile("https://api.modworkshop.net/mods/21618/download", "mods/base.zip");

		pd2hook::ExtractZIPArchive("mods/base.zip", "mods");

		if (std::filesystem::exists("mods/base.zip"))
			std::filesystem::remove("mods/base.zip");
	}
}

int luaF_directoryhash(lua_State *L)
{
	PD2HOOK_TRACE_FUNC;
	int n = lua_gettop(L);

	size_t length = 0;
	const char *filename = lua_tolstring(L, 1, &length);
	std::string hash = pd2hook::Util::GetDirectoryHash(filename);
	lua_pushlstring(L, hash.c_str(), hash.length());

	return 1;
}

int luaF_filehash(lua_State *L)
{
	int n = lua_gettop(L);
	size_t l = 0;
	const char *fileName = lua_tolstring(L, 1, &l);
	std::string hash = pd2hook::Util::GetFileHash(fileName);
	lua_pushlstring(L, hash.c_str(), hash.length());
	return 1;
}

static int HTTPReqIdent = 0;

int luaF_dohttpreq(lua_State *L)
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
	const char *url_c = lua_tolstring(L, 1, &len);
	std::string url = std::string(url_c, len);

	PD2HOOK_LOG_LOG("{} - {}", std::string(url_c, len), functionReference);

	lua_http_data *ourData = new lua_http_data();
	ourData->funcRef = functionReference;
	ourData->progressRef = progressReference;
	ourData->L = L;

	HTTPReqIdent++;
	ourData->requestIdentifier = HTTPReqIdent;

	std::unique_ptr<pd2hook::HTTPItem> reqItem(new pd2hook::HTTPItem());
	reqItem->call = return_lua_http;
	reqItem->data = ourData;
	reqItem->url = url;

	if (progressReference != 0)
	{
		reqItem->progress = progress_lua_http;
	}

	pd2hook::HTTPManager::GetSingleton()->LaunchHTTPRequest(std::move(reqItem));
	lua_pushinteger(L, HTTPReqIdent);
	return 1;
}

int luaF_createconsole(lua_State *L)
{
	Logger::Instance().OpenConsole();
	return 0;
}

int luaF_destroyconsole(lua_State *L)
{
	Logger::Instance().DestroyConsole();
	return 0;
}

int luaF_print(lua_State *L)
{
	int top = lua_gettop(L);
	std::stringstream stream;
	for (int i = 0; i < top; ++i)
	{
		size_t len;
		const char *str = lua_tolstring(L, i + 1, &len);
		stream << (i > 0 ? "    " : "") << str;
	}
	PD2HOOK_LOG_LUA(stream.str());

	return 0;
}

int luaF_moveDirectory(lua_State *L)
{
	int top = lua_gettop(L);
	size_t lf = 0;
	const char *fromStr = lua_tolstring(L, 1, &lf);
	size_t ld = 0;
	const char *destStr = lua_tolstring(L, 2, &ld);

	bool success = pd2hook::Util::MoveDirectory(fromStr, destStr);
	lua_pushboolean(L, success);
	return 1;
}

void register_lua_functions(lua_State *L)
{
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
		{"CreateConsole", luaF_createconsole},
		{"DestroyConsole", luaF_destroyconsole},
		{NULL, NULL}};
	luaI_openlib(L, "console", consoleLib, 0);

	luaL_Reg fileLib[] = {
		{"GetDirectories", luaF_getdir},
		{"GetFiles", luaF_getfiles},
		{"RemoveDirectory", luaF_removeDirectory},
		{"DirectoryExists", luaF_directoryExists},
		{"DirectoryHash", luaF_directoryhash},
		{"FileHash", luaF_filehash},
		{"MoveDirectory", luaF_moveDirectory},
		{NULL, NULL}};
	luaI_openlib(L, "file", fileLib, 0);

	// Keeping everything in lowercase since IspcallForced / IsPCallForced and Forcepcalls / ForcePCalls look rather weird anyway
	luaL_Reg bltLib[] = {
		{"ispcallforced", luaF_ispcallforced},
		{"forcepcalls", luaF_forcepcalls},
		{"GetDllVersion", luaF_GetDllVersion},
		{"EnableApplicationLog", luaF_EnableApplicationLog},
		{NULL, NULL}};
	luaI_openlib(L, "blt", bltLib, 0);

	int result;
	PD2HOOK_LOG_LOG("Initiating Hook");

	validate_mod_base();

	result = luaL_loadfilex(L, "mods/base/base.lua", nullptr);

	if (result == LUA_ERRSYNTAX)
	{
		size_t len;
		PD2HOOK_LOG_ERROR(lua_tolstring(L, -1, &len));
		return;
	}

	result = lua_pcall(L, 0, 1, 0);

	if (result == LUA_ERRRUN)
	{
		size_t len;
		PD2HOOK_LOG_LOG(lua_tolstring(L, -1, &len));
		return;
	}
}
