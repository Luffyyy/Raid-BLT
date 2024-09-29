#include "FunctionHook.h"
#include "FunctionPattern.h"

#include "http/http.h"
#include "threading/queue.h"

#include <thread>

static int32_t s_updates = 0;
std::thread::id main_thread_id;

void* (*application_update)(void* thisptr, long long llUnk0);

void* application_update_new(void* thisptr, long long llUnk0)
{
	// If someone has a better way of doing this, I'd like to know about it.
	// I could save the this pointer?
	// I'll check if it's even different at all later.
	if (std::this_thread::get_id() != main_thread_id)
	{
		return application_update(thisptr, llUnk0);
	}

	if (s_updates == 0)
	{
		if (!pd2hook::HTTPManager::GetSingleton()->AreLocksInit())
			pd2hook::HTTPManager::GetSingleton()->init_locks();

		//++s_updates;
	}

	if (s_updates > 1)
	{
		pd2hook::EventQueueMaster::GetSingleton().ProcessEvents();
	}

	s_updates++;
	return application_update(thisptr, llUnk0);
}

#define LUA_FUNCTION_PATTERN_HOOK(name, target) \
	auto name ## _hook = CreateFunctionHook(#name, target, name ## _pattern);

LUA_FUNCTION_PATTERN_HOOK(application_update, application_update_new)

void init_game()
{
	main_thread_id = std::this_thread::get_id();

	application_update_hook.GetNewFunction(application_update);
}
