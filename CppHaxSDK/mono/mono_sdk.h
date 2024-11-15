#pragma once

#include "mono_api_classes.h"
#define MONO_API_FUNC(r, n, p) extern r(*n)p
#include "mono_api_functions.h"
#undef MONO_API_FUNC
#include "mono.h"