#include "logger.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <fstream>
#include <time.h>
#include <iomanip>

namespace haxsdk {
	Logger g_logger;

	void Logger::operator<<(const haxsdk::Flush& v) {
		Flush();
	}

	Logger& Logger::LogWarning()
	{
		m_curLogLevel = LogLevel::WARNING;
		LogHeader("WARNING");
		return *this;
	}

	Logger& Logger::LogInfo()
	{
		m_curLogLevel = LogLevel::INFO;
		LogHeader("INFO");
		return *this;
	}

	Logger& Logger::LogError()
	{
		m_curLogLevel = LogLevel::ERRO;
		LogHeader("ERROR");
		return *this;
	}

	inline void Logger::LogHeader(std::string_view level) {
		Flush(); 
		auto t = std::time(nullptr);
		m_ss << std::put_time(std::localtime(&t), "%d-%m-%Y %H:%M:%S") << 
			" [" << std::left << std::setw(7) << level << "] ";
	}

	void Logger::Flush() {
		if (IsEmpty()) {
			return;
		}
		if (m_curLogLevel >= m_level) {
			m_ss << '\n';
			std::cout << m_ss.str();

			std::ofstream file(m_filePath, std::ios::app);
			file << m_ss.str();
			file.close();
		}
		m_ss.str("");
	}

	bool Logger::IsEmpty() {
		return m_ss.tellp() == std::streampos(0);
	}

	void Logger::Init(LogLevel level) {
		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

		char buff[MAX_PATH];
		GetModuleFileName(NULL, buff, MAX_PATH);
		const auto path = std::filesystem::path(buff);

		const auto logPath = path.parent_path() / "cpphaxsdk-logs.txt";
		const auto prevLogPath = path.parent_path() / "cpphaxsdk-prev-logs.txt";

		std::error_code errCode;
		std::filesystem::remove(prevLogPath, errCode);
		std::filesystem::rename(logPath, prevLogPath, errCode);
		std::filesystem::remove(logPath, errCode);

		m_filePath = logPath;
		m_level = level;
	}
}