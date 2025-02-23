#pragma once

#include <string_view>
#include <string>

namespace HaxSdk {
    void Log(std::string_view message);
    void InitLogger(bool useConsole);
}