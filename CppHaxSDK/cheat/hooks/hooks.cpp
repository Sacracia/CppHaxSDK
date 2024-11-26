#include "hooks.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef _WIN64
#include "../third_party/detours/x64/detours.h"
#else
#include "../third_party/detours/x86/detours.h"
#endif

#define MONO_API_FUNC(r, n, p)          extern r(*n)p
#define MONO_STATIC_FIELD(n, r, c)      extern r c ## _ ## n
#define MONO_FIELD_OFFSET(n, c)         extern int c ## _ ## n
#define MONO_GAME_FUNC(r, n, p, c, s)   extern r(__fastcall *c ## _ ## n)p
#include "../mono/mono_api_classes.h"
#include "../mono/mono_api_functions.h"
#include "../mono/mono_game_data.h"

namespace hooks {
    void SetupHooks() {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        DetourTransactionCommit();
    }
}