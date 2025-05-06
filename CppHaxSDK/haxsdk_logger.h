#pragma once

#include <string_view>
#include <format>

#define HAX_LOG(fmt, ...)       HaxSdk::Log(std::format(fmt, ##__VA_ARGS__))
#define HAX_LOG_ERROR(fmt, ...) HaxSdk::LogError(std::format(fmt, ##__VA_ARGS__))
#define HAX_LOG_DEBUG(fmt, ...) HaxSdk::LogDebug(std::format(fmt, ##__VA_ARGS__))
#define HAX_LOG_WARN(fmt, ...)  HaxSdk::LogWarning(std::format(fmt, ##__VA_ARGS__))

namespace HaxSdk
{
    void InitLogger(bool useConsole);
    void Log(std::string_view message);
    void LogError(std::string_view message);
    void LogWarning(std::string_view message);
    void LogDebug(std::string_view message);
}