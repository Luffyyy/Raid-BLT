#pragma once

#include <string>
#include <mutex>
#include <thread>
#include <list>
#include <memory>
#include <string_view>
#include <vector>
#include <curl/curl.h>

namespace pd2hook
{
	typedef void (*HTTPCallback)(void* data, std::string& urlContents);
	typedef void (*HTTPProgress)(void* data, long progress, long total);

	struct HTTPItem
	{
		HTTPCallback call = nullptr;
		HTTPProgress progress = nullptr;
		std::string url;
		std::string httpContents;
		void* data = nullptr;

		long byteprogress = 0;
		long bytetotal = 0;
	};

	class CURLPool
	{
	public:
		CURLPool(size_t poolSize);

		~CURLPool();

		CURL* acquire();

		void release(CURL* curl);

	private:
		std::mutex poolMutex;
		std::vector<CURL*> pool;
	};

	class HTTPManager
	{
	private:
		HTTPManager();

	public:
		~HTTPManager();

		void init_locks();

		inline bool AreLocksInit() const { return m_bLocksInit; }

		static HTTPManager* GetSingleton();
		static void Destroy(); // exit crash fix

		void SSL_Lock(int lockno);
		void SSL_Unlock(int lockno);

		void LaunchHTTPRequest(std::unique_ptr<HTTPItem> callback);

		void DownloadFile(std::string_view strUrl, std::string_view strDestination);

		CURLPool& GetCURLPool() { return curlPool; };
	private:
		CURLPool curlPool;
		std::unique_ptr<std::mutex[]> openssl_locks;
		int numLocks = 0;
		std::list<std::unique_ptr<std::thread>> threadList;
		bool m_bLocksInit;
	};
}