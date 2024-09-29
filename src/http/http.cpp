#include <curl/curl.h>
#include <openssl/crypto.h>
#include "http/http.h"
#include "threading/queue.h"
#include "util/util.h"

#include <algorithm>
#include <thread>

namespace pd2hook
{
	namespace
	{
		struct InitHttpManager
		{
			InitHttpManager() { HTTPManager::GetSingleton(); }
		};

		struct HTTPProgressNotification
		{
			HTTPItem *ourItem;
			long byteProgress;
			long byteTotal;
		};
	}

	static HTTPManager *s_httpSingleton = nullptr; // exit crash fix

	using HTTPProgressNotificationPtr = std::unique_ptr<HTTPProgressNotification>;
	using HTTPItemPtr = std::unique_ptr<HTTPItem>;
	PD2HOOK_REGISTER_EVENTQUEUE(HTTPProgressNotificationPtr, HTTPProgressNotification)
		PD2HOOK_REGISTER_EVENTQUEUE(HTTPItemPtr, HTTPItem)

		void lock_callback(int mode, int type, const char *file, int line)
	{
		if (mode & CRYPTO_LOCK)
		{
			HTTPManager::GetSingleton()->SSL_Lock(type);
		}
		else
		{
			HTTPManager::GetSingleton()->SSL_Unlock(type);
		}
	}

	HTTPManager::HTTPManager()
		: m_bLocksInit(false)
	{
		// Curl Init
		curl_global_init(CURL_GLOBAL_ALL);
		PD2HOOK_LOG_LOG("CURL_INITD");
	}

	HTTPManager::~HTTPManager()
	{
		CRYPTO_set_locking_callback(NULL);
		PD2HOOK_LOG_LOG("CURL CLOSED");
		curl_global_cleanup();

		std::for_each(threadList.begin(), threadList.end(), [](const std::unique_ptr<std::thread> &t)
			{ t->join(); });
	}

	void HTTPManager::init_locks()
	{
		PD2HOOK_TRACE_FUNC;
		numLocks = CRYPTO_num_locks();
		openssl_locks.reset(new std::mutex[numLocks]);
		CRYPTO_set_locking_callback(lock_callback);
		m_bLocksInit = true;
	}

	HTTPManager *HTTPManager::GetSingleton()
	{

		// exit crash fix
		if (s_httpSingleton == nullptr)
			s_httpSingleton = new HTTPManager();

		return s_httpSingleton;
	}

	// exit crash fix
	void HTTPManager::Destroy()
	{
		if (s_httpSingleton != nullptr)
		{
			delete s_httpSingleton;
			s_httpSingleton = nullptr;
		}
	}

	void HTTPManager::SSL_Lock(int lockno)
	{
		openssl_locks[lockno].lock();
	}

	void HTTPManager::SSL_Unlock(int lockno)
	{
		openssl_locks[lockno].unlock();
	}

	size_t write_http_data(char *ptr, size_t size, size_t nmemb, void *data)
	{
		std::string newData = std::string(ptr, size * nmemb);
		HTTPItem *mainItem = (HTTPItem *)data;
		mainItem->httpContents += newData;
		return size * nmemb;
	}

	size_t write_data_to_file(void *ptr, size_t size, size_t nmemb, FILE *stream)
	{
		size_t written = fwrite(ptr, size, nmemb, stream);
		return written;
	}

	void run_http_progress_event(std::unique_ptr<HTTPProgressNotification> ourNotify)
	{
		PD2HOOK_TRACE_FUNC;
		HTTPItem *ourItem = ourNotify->ourItem;
		ourItem->progress(ourItem->data, ourNotify->byteProgress, ourNotify->byteTotal);
	}

	int http_progress_call(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
	{
		PD2HOOK_TRACE_FUNC;
		HTTPItem *ourItem = (HTTPItem *)clientp;
		if (!ourItem->progress)
			return 0;
		if (dltotal == 0 || dlnow == 0)
			return 0;
		if (dltotal == dlnow)
			return 0;
		if (ourItem->byteprogress >= dlnow)
			return 0;
		ourItem->byteprogress = static_cast<long>(dlnow);
		ourItem->bytetotal = static_cast<long>(dltotal);

		HTTPProgressNotificationPtr notify(new HTTPProgressNotification());
		notify->ourItem = ourItem;
		notify->byteProgress = static_cast<long>(dlnow);
		notify->byteTotal = static_cast<long>(dltotal);

		GetHTTPProgressNotificationQueue().AddToQueue(run_http_progress_event, std::move(notify));
		return 0;
	}

	void run_http_event(std::unique_ptr<HTTPItem> ourItem)
	{
		PD2HOOK_TRACE_FUNC;
		ourItem->call(ourItem->data, ourItem->httpContents);
	}

	void launch_thread_http(HTTPItem *raw_item)
	{
		PD2HOOK_TRACE_FUNC;
		std::unique_ptr<HTTPItem> item(raw_item);
		CURL *curl;
		curl = curl_easy_init();
		curl_easy_setopt(curl, CURLOPT_URL, item->url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30L);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 900L);
		curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 30L);
		curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1000L);

		if (item->progress)
		{
			curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, http_progress_call);
			curl_easy_setopt(curl, CURLOPT_XFERINFODATA, item.get());
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
		}

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_http_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, item.get());

		auto res = curl_easy_perform(curl);

		try
		{
			curl_easy_cleanup(curl);
		}
		catch (std::exception e) {
			PD2HOOK_LOG_WARN("CURL cleanup failed.");
		}

		GetHTTPItemQueue().AddToQueue(run_http_event, std::move(item));
	}

	void HTTPManager::LaunchHTTPRequest(std::unique_ptr<HTTPItem> callback)
	{
		PD2HOOK_TRACE_FUNC;
		PD2HOOK_LOG_LOG("Launching Async HTTP Thread");
		// This shit's gonna end eventually, how many threads are people going to launch?
		// Probably a lot.
		// I'll manage them I guess, but I've no idea when to tell them to join which I believe is part of the constructor.

		// VC++ 2013 bug, can't pass a unique_ptr through a thread
		// should be: threadList.push_back(std::unique_ptr<std::thread>(new std::thread(launch_thread_http, std::move(callback)));
		threadList.push_back(std::unique_ptr<std::thread>(new std::thread(launch_thread_http, callback.release())));
	}

	void HTTPManager::DownloadFile(std::string_view strUrl, std::string_view strDestination)
	{
		CURL *curl;
		curl = curl_easy_init();

		auto ver = curl_version_info(CURLVERSION_NOW);
		auto test = ver->libssh_version;

		auto res = curl_easy_setopt(curl, CURLOPT_URL, strUrl.data());
		res = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		res = curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30L);
		res = curl_easy_setopt(curl, CURLOPT_TIMEOUT, 900L);
		res = curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 30L);
		res = curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1000L);

		FILE *fp = fopen(strDestination.data(), "wb");

		res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_to_file);
		res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

		res = curl_easy_perform(curl);

		try
		{
			curl_easy_cleanup(curl);
		}
		catch (std::exception e) {
			PD2HOOK_LOG_WARN("CURL cleanup failed.");
		}

		fclose(fp);
	}
}