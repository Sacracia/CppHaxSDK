#pragma once

#include <sstream>
#include <string_view>
#include <iostream>
#include <filesystem>

namespace haxsdk {
	struct Flush {};
	enum LogLevel { INFO, WARNING, ERRO };

	class Logger {
	public:
		Logger() = default;
	public:
		template <typename T>
		Logger& operator<<(const T& v) {
			m_ss << v;
			return *this;
		}
		void operator<<(const haxsdk::Flush& v);
	public:
		Logger& LogInfo();
		Logger& LogWarning();
		Logger& LogError();
	public:
		void Init(LogLevel level);
	private:
		void Flush();
		void LogHeader(std::string_view level);
		bool IsEmpty();
	public:
		const haxsdk::Flush FLUSH{};
	private:
		LogLevel m_level;
		LogLevel m_curLogLevel;
		std::ostringstream m_ss;
		std::filesystem::path m_filePath;
	};

	extern Logger g_logger;
}

#define LOG_INIT(x) haxsdk::g_logger.Init(haxsdk::LogLevel::x)
#define LOG_INFO haxsdk::g_logger.LogInfo()
#define LOG_WARNING haxsdk::g_logger.LogWarning()
#define LOG_ERROR haxsdk::g_logger.LogError()
#define LOG_FLUSH haxsdk::g_logger.FLUSH