
#ifdef MONO_GAME_FUNC
//MONO_GAME_FUNC(void, EnemyController_Death, (MonoObject* __this), "Assembly-CSharp, EnemyController:Death ()");
//MONO_GAME_FUNC(bool, Color_Inequality, (MonoObject* __this, MonoObject* lhs, MonoObject* rhs), "UnityEngine, UnityEngine.Color:op_Inequality (UnityEngine.Color,UnityEngine.Color)");

MONO_GAME_FUNC(void, HeroController_AddGeo, (MonoObject* __this, int amount), "Assembly-CSharp, HeroController:AddGeo (int)");
MONO_GAME_FUNC(int32_t, PlayerData_getCurrentMaxHealth, (MonoObject* __this), "Assembly-CSharp, PlayerData:get_CurrentMaxHealth ()");
MONO_GAME_FUNC(Vector3*, Transform_getPosition, (MonoObject* __this), "UnityEngine.CoreModule, UnityEngine.Transform:get_position ()");

#endif
