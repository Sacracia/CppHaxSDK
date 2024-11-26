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

MONO_GAME_FUNC(void, Update, (HeroController* __this), HeroController, "Update ()");

#undef MONO_ASSEMBLY
#undef MONO_NAMESPACE


#define MONO_ASSEMBLY "UnityEngine.CoreModule"
#define MONO_NAMESPACE "UnityEngine"

MONO_GAME_FUNC(Vector3, get_position, (Transform* __this), Transform, "get_position ()");
MONO_GAME_FUNC(float, get_fieldOfView, (Camera* __this), Camera, "get_fieldOfView ()");
MONO_GAME_FUNC(void, set_fieldOfView, (Camera* __this, float value), Camera, "set_fieldOfView (single)");
MONO_GAME_FUNC(Vector3, WorldToScreenPoint, (Camera* __this, Vector3), Camera, "WorldToScreenPoint (UnityEngine.Vector3)");

#undef MONO_ASSEMBLY
#undef MONO_NAMESPACE

#undef MONO_GAME_FUNC
#endif