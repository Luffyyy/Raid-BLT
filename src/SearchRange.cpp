#include "SearchRange.h"

#include <Windows.h>
#include <Psapi.h>

#ifdef min
#undef min
#endif

void* SearchRange::ms_startSearchAddress = nullptr;
size_t SearchRange::ms_searchSize = 0;

void SearchRange::Init()
{
	MODULEINFO modinfo = { 0 };
	HMODULE hModule = GetModuleHandle(nullptr);

	if (hModule == nullptr)
		return;

	GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));

	ms_startSearchAddress = modinfo.lpBaseOfDll;
	ms_searchSize = modinfo.SizeOfImage;
}

void* SearchRange::GetStartSearchAddress()
{
	if (ms_startSearchAddress == nullptr)
		Init();

	return ms_startSearchAddress;
}

size_t SearchRange::GetSearchSize()
{
	if (ms_startSearchAddress == nullptr)
		Init();

	return ms_searchSize;
}
