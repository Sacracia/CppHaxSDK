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
MONO_GAME_FUNC(void, AddHealth, (HeroController* __this, int amount), HeroController, "AddHealth (int)");
MONO_GAME_FUNC(bool, CanAttack, (HeroController* __this), HeroController, "CanAttack ()");
MONO_GAME_FUNC(void, Move, (HeroController* __this, float moveDirection), HeroController, "Move (single)");
MONO_GAME_FUNC(bool, CanDash, (HeroController* __this), HeroController, "CanDash ()");
MONO_GAME_FUNC(void, AddMPCharge, (HeroController* __this, int amount), HeroController, "AddMPCharge (int)");

MONO_GAME_FUNC(int32_t, get_CurrentMaxHealth, (PlayerData* __this), PlayerData, "get_CurrentMaxHealth ()");
MONO_GAME_FUNC(bool, get_IsInstaKillEnabled, (MonoObject* __this), CheatManager, "get_IsInstaKillEnabled ()");

#undef MONO_ASSEMBLY
#undef MONO_NAMESPACE


#define MONO_ASSEMBLY "UnityEngine.CoreModule"
#define MONO_NAMESPACE "UnityEngine"

MONO_GAME_FUNC(Vector3, get_Position, (Transform* __this), Transform, "get_position ()");
MONO_GAME_FUNC(float, get_timeScale, (MonoObject* __this), Time, "get_timeScale ()");
MONO_GAME_FUNC(void, set_timeScale, (float value), Time, "set_timeScale (single)");

#undef MONO_ASSEMBLY
#undef MONO_NAMESPACE

#undef MONO_GAME_FUNC
#endif