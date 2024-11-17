#ifdef MONO_GAME_FUNC

/*-----------------------------------------------------------------
Here you write functions you use.
You must define assembly and namespace before.
MONO_GAME_FUNC(return type, name, params, class, .net signature without class and namespace)

#define MONO_ASSEMBLY "Assembly-CSharp"
#define MONO_NAMESPACE ""
MONO_GAME_FUNC(void, AddGeo, (HeroController* __this, int amount), HeroController, "AddGeo (int)");
MONO_GAME_FUNC(void, Update, (HeroController* __this), HeroController, "Update ()");
#undef MONO_ASSEMBLY
#undef MONO_NAMESPACE
-----------------------------------------------------------------*/

#define MONO_ASSEMBLY "Assembly-CSharp"
#define MONO_NAMESPACE ""

MONO_GAME_FUNC(void, AddGeo, (HeroController* __this, int amount), HeroController, "AddGeo (int)");
MONO_GAME_FUNC(void, Update, (HeroController* __this), HeroController, "Update ()");
MONO_GAME_FUNC(int32_t, get_CurrentMaxHealth, (PlayerData* __this), PlayerData, "get_CurrentMaxHealth ()");
MONO_GAME_FUNC(bool, get_IsInstaKillEnabled, (MonoObject* __this), CheatManager, "get_IsInstaKillEnabled ()");

#undef MONO_ASSEMBLY
#undef MONO_NAMESPACE


#define MONO_ASSEMBLY "UnityEngine.CoreModule"
#define MONO_NAMESPACE "UnityEngine"

MONO_GAME_FUNC(Vector3, get_Position, (Transform* __this), Transform, "get_position ()");

#undef MONO_ASSEMBLY
#undef MONO_NAMESPACE

#undef MONO_GAME_FUNC
#endif