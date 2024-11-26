#ifdef MONO_STATIC_FIELD

/*-----------------------------------------------------------------
Here you write static fields you use.
You must define assembly and namespace before.
MONO_STATIC_FIELD(field name, return type, class)

#define MONO_ASSEMBLY "Assembly-CSharp"
#define MONO_NAMESPACE ""
MONO_STATIC_FIELD(_instance, HeroController**, HeroController);
MONO_STATIC_FIELD(_instance, PlayerData**, PlayerData);
#undef MONO_ASSEMBLY
#undef MONO_NAMESPACE

-----------------------------------------------------------------*/

#define MONO_ASSEMBLY "Assembly-CSharp"
#define MONO_NAMESPACE ""

MONO_STATIC_FIELD(_instance, HeroController**, HeroController);
MONO_STATIC_FIELD(_instance, PlayerData**, PlayerData);
MONO_STATIC_FIELD(_instance, GameCameras**, GameCameras);

#undef MONO_ASSEMBLY
#undef MONO_NAMESPACE

#undef MONO_STATIC_FIELD
#endif
