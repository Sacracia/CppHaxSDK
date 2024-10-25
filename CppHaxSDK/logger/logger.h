#pragma once

#include <sstream>
#include <string_view>
#include <iostream>
#include <filesystem>

namespace logger {
	struct Flush {};

	class Logger {
	public:
		Logger() = default;
	public:
		template <typename T>
		Logger& operator<<(const T& v) { 
			m_ss << v; 
			return *this;
		}
		void operator<<(const logger::Flush& v);
	public:
		Logger& LogInfo();
		Logger& LogWarning();
		Logger& LogError();
	public:
		void Init();
	private:
		void Flush();
		void LogHeader(std::string_view level);
		bool IsEmpty();
	public:
		const logger::Flush FLUSH{};
	private:
		std::ostringstream m_ss;
		std::filesystem::path m_filePath;
	};

	extern Logger g_logger;
}

#define LOG_INIT() logger::g_logger.Init()
#define LOG_INFO logger::g_logger.LogInfo()
#define LOG_WARNING logger::g_logger.LogWarning()
#define LOG_ERROR logger::g_logger.LogError()
#define LOG_FLUSH logger::g_logger.FLUSH