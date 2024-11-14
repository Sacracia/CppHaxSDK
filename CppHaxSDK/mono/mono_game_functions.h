
#include "mono_game_classes.h"
#include <stdint.h>

#ifdef MONO_GAME_FUNC

MONO_GAME_FUNC(void, HeroController_AddGeo, (HeroController* __this, int amount), "Assembly-CSharp, HeroController:AddGeo (int)");
MONO_GAME_FUNC(void, HeroController_Update, (HeroController* __this), "Assembly-CSharp, HeroController:Update ()");

MONO_GAME_FUNC(int32_t, PlayerData_getCurrentMaxHealth, (PlayerData* __this), "Assembly-CSharp, PlayerData:get_CurrentMaxHealth ()");
MONO_GAME_FUNC(Vector3*, Transform_getPosition, (MonoObject* __this), "UnityEngine.CoreModule, UnityEngine.Transform:get_position ()");
MONO_GAME_FUNC(bool, CheatManager_getIsInstaKillEnabled, (MonoObject* __this), "Assembly-CSharp, CheatManager:get_IsInstaKillEnabled ()");

#endif
