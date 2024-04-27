#include "FunctionPattern.h"
#include "FunctionHook.h"
#include "Lua.h"
#include "Logger.h"
#include "Disassembler.h"

#include <functional>
#include <map>

static void *s_luaAppRegisterFuncAddress = nullptr;
static std::map<uint64_t, InstructionOperand> s_luaAppRegisterFunctionInstructions;
static bool s_bEnableApplicationLog = false;

template <typename TRet, typename... TArgs>
FunctionHook<TRet, TArgs...> CreateFunctionHook(std::string_view strName,
												TRet (*targetFunc)(TArgs...), std::function<void *(uint64_t, const InstructionOperand &, void *&pUserData)> match)
{
	if (s_luaAppRegisterFuncAddress == nullptr)
	{
		SearchPattern pattern(lua_application_register_pattern);
		s_luaAppRegisterFuncAddress = pattern.Match(
			SearchRange::GetStartSearchAddress(), SearchRange::GetSearchSize());

		if (s_luaAppRegisterFuncAddress != nullptr)
		{
			PD2HOOK_LOG_LOG("Function 'lua_application_register' found -> 0x{0:016x}", reinterpret_cast<uint64_t>(s_luaAppRegisterFuncAddress));

			Dissassemble(s_luaAppRegisterFuncAddress,
						 std::min<size_t>(0x1000, SearchRange::GetSearchSize()),
						 s_luaAppRegisterFunctionInstructions);
		}
		else
		{
			PD2HOOK_LOG_ERROR("Warning: Failed to resolve 'lua_application_register'");
		}
	}

	TRet (*pFunction)(TArgs...) = nullptr;
	void *pUserData = nullptr;

	for (auto &it : s_luaAppRegisterFunctionInstructions)
	{
		pFunction = reinterpret_cast<TRet (*)(TArgs...)>(match(it.first, it.second, pUserData));

		if (pFunction != nullptr)
			break;
	}

	if (pFunction == nullptr)
	{
		PD2HOOK_LOG_LOG("Function '{0}' found -> 0x{1:016x}", strName, reinterpret_cast<uint64_t>(s_luaAppRegisterFuncAddress));
	}
	else
	{
		PD2HOOK_LOG_ERROR("Warning: Failed to resolve '{}'", strName);
	}

	return FunctionHook<TRet, TArgs...>(strName, targetFunc, pFunction);
}

static inline void *GetAddressAndName(uint64_t offset, const InstructionOperand &arg, std::string_view strName, void *&pUserData)
{
	if (arg.Instruction.mnemonic == ZYDIS_MNEMONIC_LEA)
	{
		if (arg.Operands[0].reg.value == ZYDIS_REGISTER_R8)
		{
			auto newOffset = (offset + arg.Operands[1].mem.disp.value) + arg.Instruction.length;
			std::string_view strFuncName = reinterpret_cast<const char *>(newOffset);

			if (strFuncName == strName)
				return pUserData;
		}
		else if (arg.Operands[0].reg.value == ZYDIS_REGISTER_RDX)
		{
			pUserData = reinterpret_cast<void *>((offset + arg.Operands[1].mem.disp.value) + arg.Instruction.length);
		}
	}

	return nullptr;
}

int lua_application_debug(lua_State *L)
{
	if (!s_bEnableApplicationLog)
		return 0;

	std::string strMessage = "LUA DEBUG: ";
	strMessage += lua_tostring(L, 2);

	PD2HOOK_LOG_LUA_DEBUG(strMessage.c_str());

	return 0;
}

int lua_application_trace(lua_State *L)
{
	if (!s_bEnableApplicationLog)
		return 0;

	std::string strMessage = "LUA TRACE: ";
	strMessage += lua_tostring(L, 2);

	PD2HOOK_LOG_LUA_TRACE(strMessage.c_str());

	return 0;
}

int lua_application_info(lua_State *L)
{
	if (!s_bEnableApplicationLog)
		return 0;

	std::string strMessage = "LUA INFO: ";
	strMessage += lua_tostring(L, 2);

	PD2HOOK_LOG_LUA_INFO(strMessage.c_str());

	return 0;
}

int lua_application_warn(lua_State *L)
{
	if (!s_bEnableApplicationLog)
		return 0;

	std::string strMessage = "LUA WARN: ";
	strMessage += lua_tostring(L, 2);

	PD2HOOK_LOG_LUA_WARN(strMessage.c_str());

	return 0;
}

int lua_application_error(lua_State *L)
{
	if (!s_bEnableApplicationLog)
		return 0;

	std::string strMessage = "LUA ERROR: ";
	strMessage += lua_tostring(L, 2);

	PD2HOOK_LOG_LUA_ERROR(strMessage.c_str());

	return 0;
}

int lua_application_log(lua_State *L)
{
	if (!s_bEnableApplicationLog)
		return 0;

	std::string strMessage = "LUA ERROR: ";
	strMessage += lua_tostring(L, 2);

	PD2HOOK_LOG_LUA_LOG(strMessage.c_str());

	return 0;
}

void EnableApplicationLog(bool value)
{
	s_bEnableApplicationLog = value;
}

#define LUA_FUNCTION_HOOK(name, target, targetName)      \
	auto name##_hook = CreateFunctionHook(#name, target, \
										  [](uint64_t offset, const InstructionOperand &arg, void *&pUserData) { return GetAddressAndName(offset, arg, targetName, pUserData); });

LUA_FUNCTION_HOOK(lua_app_debug, lua_application_debug, "debug");
LUA_FUNCTION_HOOK(lua_app_trace, lua_application_trace, "trace");
LUA_FUNCTION_HOOK(lua_app_info, lua_application_info, "info");
LUA_FUNCTION_HOOK(lua_app_warn, lua_application_warn, "warn");
LUA_FUNCTION_HOOK(lua_app_error, lua_application_error, "error");
LUA_FUNCTION_HOOK(lua_app_log, lua_application_log, "log");
