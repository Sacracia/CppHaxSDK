#pragma once

#include <sstream>
#include <string_view>
#include <iostream>
#include <filesystem>

namespace haxsdk {
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
		void operator<<(const haxsdk::Flush& v);
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
		const haxsdk::Flush FLUSH{};
	private:
		std::ostringstream m_ss;
		std::filesystem::path m_filePath;
	};

	extern Logger g_logger;
}

#define LOG_INIT() haxsdk::g_logger.Init()
#define LOG_INFO haxsdk::g_logger.LogInfo()
#define LOG_WARNING haxsdk::g_logger.LogWarning()
#define LOG_ERROR haxsdk::g_logger.LogError()
#define LOG_FLUSH haxsdk::g_logger.FLUSH