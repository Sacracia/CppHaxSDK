#include <stdint.h>

#ifdef MONO_GAME_FUNC

MONO_GAME_FUNC(void, HeroController_AddGeo, (MonoObject* __this, int amount), "Assembly-CSharp, HeroController:AddGeo (int)");
MONO_GAME_FUNC(void, HeroController_Update, (MonoObject* __this), "Assembly-CSharp, HeroController:Update ()");

MONO_GAME_FUNC(int32_t, PlayerData_getCurrentMaxHealth, (MonoObject* __this), "Assembly-CSharp, PlayerData:get_CurrentMaxHealth ()");
MONO_GAME_FUNC(MonoObject*, Transform_getPosition, (MonoObject* __this), "UnityEngine.CoreModule, UnityEngine.Transform:get_position ()");
MONO_GAME_FUNC(bool, CheatManager_getIsInstaKillEnabled, (MonoObject* __this), "Assembly-CSharp, CheatManager:get_IsInstaKillEnabled ()");

#endif
