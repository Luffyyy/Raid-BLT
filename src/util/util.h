#ifndef __UTIL_HEADER__
#define __UTIL_HEADER__

#include <exception>
#include <vector>
#include <string>
#include <sstream>
#include <windows.h>

#include "Logger.h"

namespace pd2hook
{

namespace Util {
	std::vector<std::string> GetDirectoryContents(const std::string& path, bool isDirs = false);
	std::string GetFileContents(const std::string& filename);
	void EnsurePathWritable(const std::string& path);
	bool RemoveEmptyDirectory(const std::string& dir);
	bool DirectoryExists(const std::string& dir);
	bool CreateDirectoryPath(const std::string& dir);
	// String split from http://stackoverflow.com/a/236803
	void SplitString(const std::string &s, char delim, std::vector<std::string> &elems);
	std::vector<std::string> SplitString(const std::string &s, char delim);
    std::string GetDirectoryHash(std::string directory);
	std::string GetFileHash(std::string filename);
	bool MoveDirectory(const std::string & path, const std::string & destination);

	class Exception : public std::exception
	{
	public:
		Exception(const char *file, int line);
		Exception(std::string msg, const char *file, int line);

		virtual const char *what() const override;

		virtual const char *exceptionName() const;
		virtual void writeToStream(std::ostream& os) const;

	private:
		const char * const mFile;
		const int mLine;
		const std::string mMsg;
	};

#define PD2HOOK_SIMPLE_THROW() throw pd2hook::Util::Exception(__FILE__, __LINE__)
#define PD2HOOK_SIMPLE_THROW_MSG(msg) throw pd2hook::Util::Exception(msg, __FILE__, __LINE__)

	inline std::ostream& operator<<(std::ostream& os, const Exception& e)
	{
		e.writeToStream(os);
		return os;
	}

	class IOException : public Exception
	{
	public:
		IOException(const char *file, int line);
		IOException(std::string msg, const char *file, int line);

		virtual const char *exceptionName() const;
	};

#define PD2HOOK_THROW_IO() throw pd2hook::Util::IOException(__FILE__, __LINE__)
#define PD2HOOK_THROW_IO_MSG(msg) throw pd2hook::Util::IOException(msg, __FILE__, __LINE__)
}


bool ExtractZIPArchive(const std::string& path, const std::string& extractPath);
}

#ifdef PD2HOOK_ENABLE_FUNCTION_TRACE
#define PD2HOOK_TRACE_FUNC pd2hook::Logging::FunctionLogger funcLogger(__FUNCTION__, __FILE__)
#define PD2HOOK_TRACE_FUNC_MSG(msg) pd2hook::Logging::FunctionLogger funcLogger(msg)
#else
#define PD2HOOK_TRACE_FUNC
#define PD2HOOK_TRACE_FUNC_MSG(msg)
#endif

#define PD2HOOK_LOG_LEVEL(msg, level, file, line, color) do { \
	auto& logger = pd2hook::Logging::Logger::Instance(); \
	if(level >= logger.getLoggingLevel()) { \
		if (color > 0x0000) \
		{ \
			HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE); \
			SetConsoleTextAttribute(hStdout, color); \
		} \
		pd2hook::Logging::LogWriter writer(file, line, level); \
		writer << msg; \
		writer.write(logger); \
	}} while (false);

#define PD2HOOK_DEBUG_CHECKPOINT PD2HOOK_LOG_LOG("Checkpoint")

#endif // __UTIL_HEADER__
