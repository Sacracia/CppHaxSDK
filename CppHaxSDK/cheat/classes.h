#pragma once

#include "../core/core.h"
#define MONO_FUNCTION(a, n, c, m, s)     extern MonoMethodWrapper c ## __ ## m
#define MONO_STATIC_FIELD(a, n, c, f, t) extern t* c ## __ ## f
#define MONO_FIELD_OFFSET(a, n, c, f)    extern int c ## __ ## f
#include "../cheat/game_data.h"
#undef MONO_FUNCTION
#undef MONO_STATIC_FIELD
#undef MONO_FIELD_OFFSET