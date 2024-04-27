#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <string_view>
#include <fstream>

#include <format>
#include <mutex>

#include "console/console.h"

enum class LogType
{
	Debug,
	Trace,
	Log,
	Lua,
	Warn,
	Error,
};

class Logger
{
public:
	virtual ~Logger();

	static Logger& Instance();
	static void DestroyInstance();

	void Log(LogType type, std::string_view strText, std::string_view file, int32_t line);

	void OpenConsole();
	void DestroyConsole();

	void Close();

	template <typename... TArgs>
	void Log(LogType type, std::string_view strText, std::string_view file, int32_t line, TArgs&&... args)
	{
		auto str = std::vformat(strText, std::make_format_args(args...));
		Log(type, str, file, line);
	}

protected:
	virtual void WriteMessage(LogType type, std::string_view strFullFormattedText, std::string_view strFormattedText);

private:
	Logger(std::string_view strDirectory);

	LogType m_currentLogLevel;
	std::ofstream m_logFileStream;
	std::unique_ptr<CConsole> m_pConsole;
	std::mutex m_mtxLog;

	static std::mutex m_mtxCreateLog;
	static Logger* ms_instance;
};

#define PD2HOOK_LOG_LOG(msg, ...) Logger::Instance().Log(LogType::Log, msg, __FILE__, __LINE__, __VA_ARGS__)
#define PD2HOOK_LOG_LUA(msg, ...) Logger::Instance().Log(LogType::Lua, msg, __FILE__, __LINE__, __VA_ARGS__)
#define PD2HOOK_LOG_WARN(msg, ...) Logger::Instance().Log(LogType::Warn, msg, __FILE__, __LINE__, __VA_ARGS__)
#define PD2HOOK_LOG_ERROR(msg, ...) Logger::Instance().Log(LogType::Error, msg, __FILE__, __LINE__, __VA_ARGS__)
#define PD2HOOK_LOG_EXCEPTION(e) PD2HOOK_LOG_WARN(e)

#define PD2HOOK_LOG_LUA_DEBUG(msg) Logger::Instance().Log(LogType::Debug, msg, __FILE__, __LINE__)
#define PD2HOOK_LOG_LUA_TRACE(msg) Logger::Instance().Log(LogType::Trace, msg, __FILE__, __LINE__)
#define PD2HOOK_LOG_LUA_INFO(msg) PD2HOOK_LOG_LOG(msg)
#define PD2HOOK_LOG_LUA_WARN(msg) PD2HOOK_LOG_WARN(msg)
#define PD2HOOK_LOG_LUA_ERROR(msg) PD2HOOK_LOG_ERROR(msg)
#define PD2HOOK_LOG_LUA_LOG(msg) PD2HOOK_LOG_LOG(msg)

#endif
