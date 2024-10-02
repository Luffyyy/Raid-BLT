#pragma once

class SearchRange
{
public:
	static void* GetStartSearchAddress();
	static size_t GetSearchSize();

private:
	static void* ms_startSearchAddress;
	static size_t ms_searchSize;

	static void Init();
};