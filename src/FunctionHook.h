#pragma once

#include "Logger.h"
#include "SearchRange.h"
#include "SearchPattern.h"

#include <MinHook.h>

#include <vector>

class FunctionHookManager
{
public:
	FunctionHookManager();
	virtual ~FunctionHookManager();

	virtual bool Initialize() = 0;
	virtual void Destroy() = 0;

	static bool Setup();
	static void DestroyAll();

private:
	static std::vector<FunctionHookManager*> ms_functionHooks;
};

template <typename TRet, typename... TArgs>
class FunctionHook : public FunctionHookManager
{
public:
	FunctionHook()
		: m_pHookAddress(nullptr)
	{ }

	FunctionHook(std::string_view strFuncName, TRet(*pHookAddress)(TArgs...), TRet(*pSrcAddress)(TArgs...))
		: m_strFuncName(strFuncName), m_pHookAddress(pHookAddress), m_pOriginalAddress(pSrcAddress), m_pNewFunction(nullptr)
	{ }

	virtual ~FunctionHook()
	{
		Destroy();
	}

	bool IsInitialized() const { return m_pNewFunction != nullptr; }

	bool Initialize() override
	{
		if (m_pOriginalAddress == nullptr)
			return false;

		void* pTrampoline = nullptr;

		auto status = MH_CreateHook(m_pOriginalAddress, m_pHookAddress, &pTrampoline);

		if (status != MH_OK)
		{
			PD2HOOK_LOG_ERROR("Warning: MH_CreateHook(0x{0:016x}, 0x{1:016x}, NULL) failed with error | {2}", reinterpret_cast<uint64_t>(m_pOriginalAddress), reinterpret_cast<uint64_t>(m_pHookAddress), MH_StatusToString(status));
			return false;
		}

		status = MH_EnableHook(m_pOriginalAddress);

		if (status != MH_OK)
		{
			PD2HOOK_LOG_ERROR("Warning: MH_EnableHook(0x{0:016x}) failed with error | {1}", reinterpret_cast<uint64_t>(m_pOriginalAddress), MH_StatusToString(status));
			return false;
		}

		m_pNewFunction = reinterpret_cast<TRet(*)(TArgs...)>(pTrampoline);

		return true;
	}

	void Destroy() override
	{
		if (m_pNewFunction != nullptr)
		{
			auto status = MH_RemoveHook(m_pOriginalAddress);

			if (status != MH_OK)
			{
				PD2HOOK_LOG_ERROR("Warning: MH_RemoveHook(0x{0:016x}) failed with error | {1}", reinterpret_cast<uint64_t>(m_pOriginalAddress), MH_StatusToString(status));
			}

			m_pNewFunction = nullptr;
		}
	}

	inline void GetNewFunction(TRet(*&pFunc)(TArgs...)) const { pFunc = m_pNewFunction; }

private:
	std::string_view m_strFuncName;

	TRet(*m_pOriginalAddress)(TArgs...);
	TRet(*m_pHookAddress)(TArgs...);
	TRet(*m_pNewFunction)(TArgs...);
};

template<typename TRet, typename... TArgs>
inline FunctionHook<TRet, TArgs...> CreateFunctionHook(std::string_view strName, TRet(*targetFunc)(TArgs...), std::string_view strPattern)
{
	SearchPattern pattern(strPattern);

	void* pAddress = pattern.Match(SearchRange::GetStartSearchAddress(), SearchRange::GetSearchSize());

	if (pAddress != nullptr)
	{
		PD2HOOK_LOG_LOG("Function '{0}' found -> 0x{1:016x}", strName, reinterpret_cast<uint64_t>(pAddress));
	}
	else
	{
		PD2HOOK_LOG_ERROR("Warning: Failed to resolve '{}'", strName);
	}

	return FunctionHook<TRet, TArgs...>(strName, targetFunc, reinterpret_cast<TRet(*)(TArgs...)>(pAddress));
}
