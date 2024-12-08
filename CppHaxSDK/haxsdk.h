#pragma once

#if !defined(HAX_MONO) && !defined(HAX_IL2CPP)
#error Either HAX_MONO or HAX_IL2CPP must be specified
#endif

#include "logger/logger.h"
#include "haxsdk_gui.h"
#include "unity/haxsdk_unity.h"