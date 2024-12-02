#include "logger.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <fstream>
#include <time.h>
#include <ctime>
#include <iomanip>
#include <mutex>

namespace haxsdk {
	Logger g_logger;

	void Logger::operator<<(const haxsdk::Flush& v) {
		Flush();
        m_mutex.unlock();
	}

	Logger& Logger::LogDebug()
	{
		m_curLogLevel = LogLevel::DEBUG;
		LogHeader("DEBUG");
		return *this;
	}

	Logger& Logger::LogInfo()
	{
		m_curLogLevel = LogLevel::INFO;
		LogHeader("INFO");
		return *this;
	}

	Logger& Logger::LogWarning()
	{
		m_curLogLevel = LogLevel::WARNING;
		LogHeader("WARNING");
		return *this;
	}

	Logger& Logger::LogError()
	{
		m_curLogLevel = LogLevel::ERRO;
		LogHeader("ERROR");
		return *this;
	}

	void Logger::LogHeader(std::string_view level) {
		m_mutex.lock();
		auto t = std::time(nullptr);
        struct tm newtime;
        ::localtime_s(&newtime, &t);
		m_ss << std::put_time(&newtime, "%d-%m-%Y %H:%M:%S") << " [" << std::left << std::setw(7) << level << "] ";
	}

	void Logger::Flush() {
		if (IsEmpty()) {
			return;
		}
		if (m_curLogLevel >= m_level) {
			m_ss << '\n';
            if (m_useConsole) {
                std::cout << m_ss.str();
            }

			std::ofstream file(m_filePath, std::ios::app);
			file << m_ss.str();
			file.close();
		}
		m_ss.str("");
	}

	bool Logger::IsEmpty() {
		return m_ss.tellp() == std::streampos(0);
	}

	void Logger::Init(LogLevel level, bool useConsole = false) {
        m_useConsole = useConsole;
        if (useConsole) {
            AllocConsole();
            freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        }

		char buff[MAX_PATH];
		GetModuleFileName(NULL, buff, MAX_PATH);
		const auto path = std::filesystem::path(buff);

		const auto logPath = path.parent_path() / "haxsdk-logs.txt";
		const auto prevLogPath = path.parent_path() / "haxsdk-prev-logs.txt";

		std::error_code errCode;
		std::filesystem::remove(prevLogPath, errCode);
		std::filesystem::rename(logPath, prevLogPath, errCode);
		std::filesystem::remove(logPath, errCode);

		m_filePath = logPath;
		m_level = level;
	}
}