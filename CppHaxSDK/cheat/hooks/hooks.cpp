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

extern bool isInstaKill;
extern bool isInvincible;
extern bool isFastAttack;
extern bool isInfiniteDash;
extern bool isInfiniteSoul;
extern float speedMultiplier;
extern int geoMultiplier;

static bool HookedCheatManager_getIsInstaKillEnabled(MonoObject* __this) {
    return isInstaKill ? true : game::funcs::CheatManager_get_IsInstaKillEnabled(__this);
}

static void HookedHeroController_Update(HeroController* __this) {
    PlayerData* pPlayerData = __this->playerData();
    pPlayerData->isInvincible() = isInvincible;

    if (isInfiniteSoul) {
        int curMp = pPlayerData->MPCharge();
        int maxMp = pPlayerData->maxMP();
        if (curMp < maxMp)
            __this->AddMPCharge(maxMp - curMp);
    }

    return game::funcs::HeroController_Update(__this);
}

static bool HookedHeroController_CanAttack(HeroController* __this) {
    return isFastAttack ? true : game::funcs::HeroController_CanAttack(__this);
}

static void HookedHeroController_Move(HeroController* __this, float moveDirection) {
    game::funcs::HeroController_Move(__this, moveDirection * speedMultiplier);
}

static bool HookedHeroController_CanDash(HeroController* __this) {
    return isInfiniteDash ? true : game::funcs::HeroController_CanDash(__this);
}

static void HookedHeroController_AddGeo(HeroController* __this, int amount) {
    game::funcs::HeroController_AddGeo(__this, amount * geoMultiplier);
}

namespace hooks {
    void SetupHooks() {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)game::funcs::CheatManager_get_IsInstaKillEnabled, HookedCheatManager_getIsInstaKillEnabled);
        DetourAttach(&(PVOID&)game::funcs::HeroController_Update, HookedHeroController_Update);
        DetourAttach(&(PVOID&)game::funcs::HeroController_CanAttack, HookedHeroController_CanAttack);
        DetourAttach(&(PVOID&)game::funcs::HeroController_Move, HookedHeroController_Move);
        DetourAttach(&(PVOID&)game::funcs::HeroController_CanDash, HookedHeroController_CanDash);
        DetourAttach(&(PVOID&)game::funcs::HeroController_AddGeo, HookedHeroController_AddGeo);
        DetourTransactionCommit();
    }
}