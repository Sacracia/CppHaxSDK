#include "hooks.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "../mono/mono_sdk.h"

#ifdef _WIN64
#include "../third_party/detours/x64/detours.h"
#else
#include "../third_party/detours/x86/detours.h"
#endif

namespace hero_controller {
    namespace static_fields {
        extern void* _instance;
    }

    namespace offsets {
        extern int playerData;
        extern int transform;
    }

    namespace funcs {
        extern void(*HeroController_AddGeo)(MonoObject* __this, int amount);
        extern void(*HeroController_Update)(MonoObject* __this);
    }
}

namespace player_data {
    namespace static_fields {
        extern void* _instance;
    }

    namespace offsets {
        extern int infiniteAirJump;
        extern int isInvincible;
        extern int scenesVisited;
    }

    namespace funcs {
        extern int32_t(*PlayerData_getCurrentMaxHealth)(MonoObject* __this);
    }
}

namespace transform {
    namespace funcs {
        extern MonoObject* (*Transform_getPosition)(MonoObject* __this);
    }
}

namespace cheat_manager {
    namespace funcs {
        extern bool(*CheatManager_getIsInstaKillEnabled)(MonoObject* __this);
    }
}

#include "../game/game_classes.h"

extern bool isInstaKill;
extern bool isInvincible;

static bool HookedCheatManager_getIsInstaKillEnabled(MonoObject* __this) {
    return isInstaKill ? true : cheat_manager::funcs::CheatManager_getIsInstaKillEnabled(__this);
}

static void HookedHeroController_Update(HeroController* __this) {
    PlayerData* pPlayerData = __this->playerData();
    pPlayerData->isInvincible() = isInvincible;
    return hero_controller::funcs::HeroController_Update(__this);
}

namespace hooks {
    void SetupHooks() {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)cheat_manager::funcs::CheatManager_getIsInstaKillEnabled, HookedCheatManager_getIsInstaKillEnabled);
        DetourAttach(&(PVOID&)hero_controller::funcs::HeroController_Update, HookedHeroController_Update);
        DetourTransactionCommit();
    }
}