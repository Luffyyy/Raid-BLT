#include "Logger.h"
#include "StringConstants.h"

#include <chrono>
#include <filesystem>
#include <format>

#include <Windows.h>

Logger* Logger::ms_instance = nullptr;
std::mutex Logger::m_mtxCreateLog;

inline std::string_view GetLogTypeString(LogType logType)
{
	switch (logType)
	{
	case LogType::Debug:
		return "DEBUG";
	case LogType::Trace:
		return "TRACE";
	case LogType::Log:
		return "Log";
	case LogType::Lua:
		return "Lua";
	case LogType::Warn:
		return "WARNING";
	case LogType::Error:
		return "FATAL ERROR";
	default:
		return "Message";
	}
}

static std::string GetDateString()
{
	std::time_t currentTime = time(0);
	std::tm now;
	localtime_s(&now, &currentTime);

	char datestring[100];
	std::strftime(datestring, sizeof(datestring), "%Y_%m_%d", &now);
	return datestring;
}

Logger::Logger(std::string_view strDirectory)
	: m_currentLogLevel(LogType::Debug)
{ 
	if (ms_instance == nullptr)
		ms_instance = this;

	if (!std::filesystem::exists(strDirectory))
		std::filesystem::create_directories(strDirectory);

	std::string path(strDirectory);
	path += "/";
	path += GetDateString();
	path += "_log.txt";

	m_logFileStream = std::ofstream(path.c_str(), std::ios::app);
}

Logger::~Logger()
{ }

Logger& Logger::Instance()
{ 
	if (ms_instance == nullptr)
	{
		std::lock_guard<std::mutex> lck(m_mtxCreateLog); // lock only needed if instance is null

		if (ms_instance == nullptr) // check again for instance
			ms_instance = new Logger(LogDirectory);
	}

	return *ms_instance;
}

void Logger::DestroyInstance()
{
	std::lock_guard<std::mutex> lck(m_mtxCreateLog);

	if (ms_instance != nullptr)
		delete ms_instance;
}

void Logger::Close()
{
	std::lock_guard<std::mutex> lck(m_mtxLog);
	m_logFileStream.close();
}

void Logger::OpenConsole()
{
	std::lock_guard<std::mutex> lck(m_mtxLog);

	if (m_pConsole != nullptr)
		return;

	m_pConsole = std::make_unique<CConsole>();
}

void Logger::DestroyConsole()
{
	std::lock_guard<std::mutex> lck(m_mtxLog);
	m_pConsole = nullptr;
}

void Logger::Log(LogType type, std::string_view strText, std::string_view file, int32_t line)
{
	if (type < m_currentLogLevel)
		return;

    auto time = std::chrono::system_clock::now();
    std::string_view logType = GetLogTypeString(type);

	std::string strMessage = std::vformat("{0:%I}:{0:%M}:{0:%OS} {0:%p} {1}: {2}", std::make_format_args(time, logType, strText));

	if (!file.empty() && line > 0)
	{
		auto str = std::vformat("{0:%I}:{0:%M}:{0:%OS} {0:%p} {1}: ({2}:{3}) {4}", std::make_format_args(time, logType, file, line, strText));
		WriteMessage(type, str, strMessage);
	}
	else if (!file.empty())
	{
		auto str = std::vformat("{0:%I}:{0:%M}:{0:%OS} {0:%p} {1}: ({2}) {3}", std::make_format_args(time, logType, file, strText));
		WriteMessage(type, str, strMessage);
	}
	else
	{
		auto str = std::vformat("{0:%I}:{0:%M}:{0:%OS} {0:%p} {1}: {2}", std::make_format_args(time, logType, strText));
		WriteMessage(type, str, strMessage);
	}
}

void Logger::WriteMessage(LogType type, std::string_view strFullFormattedText, std::string_view strFormattedText)
{
	std::lock_guard<std::mutex> lck(m_mtxLog);

	if (m_logFileStream.is_open())
		m_logFileStream << strFullFormattedText << std::endl; // we want the log message immediately to prevent message loss after a crash

	if (m_pConsole != nullptr)
	{
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

		switch (type)
		{
		case LogType::Log:
			SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case LogType::Lua:
			SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case LogType::Warn:
			SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case LogType::Error:
			SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		default:
			SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		}

		printf("%s\n", strFormattedText.data());
	}
}

LogType Logger::getLoggingLevel() {
	return m_currentLogLevel;
}