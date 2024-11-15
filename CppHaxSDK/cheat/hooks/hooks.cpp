#include "hooks.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define MONO_GAME_FUNC(r, n, p, s) extern r(*n)p
#include "../game/game_classes.h"
#include "../game/game_functions.h"
#undef MONO_GAME_FUNC

#ifdef _WIN64
#include "../third_party/detours/x64/detours.h"
#else
#include "../third_party/detours/x86/detours.h"
#endif

extern bool isInstaKill;
extern bool isInvincible;

static bool HookedCheatManager_getIsInstaKillEnabled(MonoObject* __this) {
    return isInstaKill ? true : CheatManager_getIsInstaKillEnabled(__this);
}

static void HookedHeroController_Update(HeroController* __this) {
    PlayerData* pPlayerData = *__this->playerData();
    *pPlayerData->isInvincible() = isInvincible;
    return HeroController_Update(__this);
}

namespace hooks {
    void SetupHooks() {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)CheatManager_getIsInstaKillEnabled, HookedCheatManager_getIsInstaKillEnabled);
        DetourAttach(&(PVOID&)HeroController_Update, HookedHeroController_Update);
        DetourTransactionCommit();
    }
}