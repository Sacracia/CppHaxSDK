#pragma once

#ifdef HAX_MONO
#include "backends/haxsdk_mono.h"
#elif defined(HAX_IL2CPP)
#include "backends/haxsdk_il2cpp.h"
#endif

#include "haxsdk_gui.h"
#include "logger/logger.h"