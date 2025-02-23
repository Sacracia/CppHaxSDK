#include "logger.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <iostream>
#include <fstream>
#include <time.h>
#include <ctime>
#include <format>
#include <sstream>
#include <filesystem>

class Logger {
public:
    void Log(std::string_view message);
    void Init(bool useConsole);
private:
    std::filesystem::path m_filePath;
    bool m_initialized = false;
    bool m_useConsole = false;
};

void Logger::Log(std::string_view message) {
    if (!m_initialized)
        return;

    auto t = std::time(nullptr);
    struct tm newtime;
    ::localtime_s(&newtime, &t);
    std::stringstream ss{};
    ss << std::put_time(&newtime, "%d-%m-%Y %H:%M:%S") << " [" << std::left << std::setw(7) << "DEBUG" << "] " << message;

    std::ofstream file(m_filePath, std::ios_base::app);
    file << ss.str();
    file.close();

    if (m_useConsole)
        std::cout << ss.str();
}

void Logger::Init(bool useConsole) {
    if (m_initialized)
        return;

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
    m_initialized = true;
}

namespace HaxSdk {
    static Logger g_logger;

    void InitLogger(bool useConsole) {
        g_logger.Init(useConsole);
    }

    void Log(std::string_view message) {
        g_logger.Log(message);
    }
}