#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <MinHook.h>

#include "signatures.h"

MODULEINFO GetModuleInfo(std::string szModule)
{
	MODULEINFO modinfo = { 0 };
	HMODULE hModule = GetModuleHandle(szModule.c_str());
	if (hModule == 0)
		return modinfo;
	GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));
	return modinfo;
}


unsigned __int64 FindPattern(char* module, const char* funcname, const char* pattern, const char* mask)
{
	MODULEINFO mInfo = GetModuleInfo(module);
	DWORDLONG base = (DWORDLONG)mInfo.lpBaseOfDll;
	DWORDLONG size = (DWORDLONG)mInfo.SizeOfImage;
	DWORDLONG patternLength = (DWORDLONG)strlen(mask);
	for (DWORDLONG i = 0; i < size - patternLength; i++){
		bool found = true;
		for (DWORDLONG j = 0; j < patternLength; j++){
			found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
		}
		if (found) {
			//			printf("Found %s: 0x%p\n", funcname, base + i);
			return base + i;
		}
	}
	printf("Warning: Failed to locate function %s\n", funcname);
	return NULL;
}

std::vector<SignatureF>* allSignatures = NULL;

SignatureSearch::SignatureSearch(const char* funcname, void* adress, const char* signature, const char* mask, int offset){
	// lazy-init, container gets 'emptied' when initialized on compile.
	if (!allSignatures){
		allSignatures = new std::vector<SignatureF>();
	}

	SignatureF ins = { funcname, signature, mask, offset, adress };
	allSignatures->push_back(ins);
}

void SignatureSearch::Search(){
	printf("Scanning for signatures.\n");
	std::vector<SignatureF>::iterator it;
	for (it = allSignatures->begin(); it < allSignatures->end(); it++){
		*((void**)it->address) = (void*)(FindPattern("raid_win64_release.exe", it->funcname, it->signature, it->mask) + it->offset);
	}
}


FuncDetour::FuncDetour(void* oldF, void* newF) : oldFunction(oldF), newFunction(newF){
	void* trampoline = nullptr;

	MH_STATUS status = MH_CreateHook(oldF, newF, reinterpret_cast<void**>(&trampoline));
	if (status != MH_OK){
		// TODO: Proper error handling, not this sloppy crap
		printf("Warning: MH_CreateHook(%p, %p, NULL) failed with error | %s\n", oldF, newF, MH_StatusToString(status));
		return;
	}

	status = MH_EnableHook(oldF);
	if (status != MH_OK){
		// TODO: Proper error handling, not this sloppy crap
		printf("Warning: MH_EnableHook(%p) failed with error | %s\n", oldF, MH_StatusToString(status));
	}

	trampolineFunction = trampoline;
}

FuncDetour::~FuncDetour(){
	MH_STATUS status = MH_RemoveHook(oldFunction);
	if (status != MH_OK){
		// TODO: Proper error handling, not this sloppy crap
		printf("Warning: MH_RemoveHook(%p) failed with error | %s\n", oldFunction, MH_StatusToString(status));
		//		return;
	}

	oldFunction = nullptr;
	newFunction = nullptr;
	trampolineFunction = nullptr;
}

void* FuncDetour::GetTrampoline(){
	return trampolineFunction;
}

static bool MinHookInitialized = false;

bool InitializeMinHook(){
	if (MinHookInitialized){
		return true;
	}

	MH_STATUS status = MH_Initialize();
	if (status != MH_OK){
		printf("Error: MH_Initialize() failed with error | %s\n", MH_StatusToString(status));
		return false;
	}

	MinHookInitialized = true;
	return true;
}

void UninitializeMinHook(){
	if (!MinHookInitialized){
		return;
	}

	MH_STATUS status = MH_Uninitialize();
	if (status != MH_OK){
		printf("Error: MH_Uninitialize() failed with error | %s\n", MH_StatusToString(status));
		return;
	}
	MinHookInitialized = false;
}
