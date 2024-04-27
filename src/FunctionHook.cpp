#include "FunctionHook.h"

std::vector<FunctionHookManager*> FunctionHookManager::ms_functionHooks;

FunctionHookManager::FunctionHookManager()
{ 
	ms_functionHooks.push_back(this);
}

FunctionHookManager::~FunctionHookManager()
{ }

bool FunctionHookManager::Setup()
{
	auto status = MH_Initialize();
	if (status != MH_OK)
	{
		PD2HOOK_LOG_ERROR("Error: MH_Initialize() failed with error | {}", MH_StatusToString(status));
		return false;
	}

	for (auto& it : ms_functionHooks)
	{
		if (!it->Initialize())
			return false;
	}

	return true;
}

void FunctionHookManager::DestroyAll()
{
	for (auto& it : ms_functionHooks)
		it->Destroy();

	auto status = MH_Uninitialize();

	if (status != MH_OK)
	{
		PD2HOOK_LOG_ERROR("Error: MH_Uninitialize() failed with error | {}", MH_StatusToString(status));
		return;
	}
}
