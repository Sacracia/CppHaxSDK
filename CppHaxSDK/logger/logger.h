#pragma once

#include <string_view>
#include <iostream>
#include <string>
#include <filesystem>
#include <format>

namespace HaxSdk {
    void Log(std::string_view message);
    void InitLogger(bool useConsole);
}