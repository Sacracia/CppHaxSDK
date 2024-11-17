#ifdef MONO_FIELD_OFFSET

/*-----------------------------------------------------------------
Here you write nonstatic fields you use.
You must define assembly and namespace before.
MONO_FIELD_OFFSET(name, class);

#define MONO_ASSEMBLY "Assembly-CSharp"
#define MONO_NAMESPACE ""
MONO_FIELD_OFFSET(playerData, HeroController);
MONO_FIELD_OFFSET(transform, HeroController);
#undef MONO_ASSEMBLY
#undef MONO_NAMESPACE
-----------------------------------------------------------------*/

#define MONO_ASSEMBLY "Assembly-CSharp"
#define MONO_NAMESPACE ""

// HeroController
MONO_FIELD_OFFSET(playerData, HeroController);
MONO_FIELD_OFFSET(transform, HeroController);

// PlayerData
MONO_FIELD_OFFSET(infiniteAirJump, PlayerData);
MONO_FIELD_OFFSET(isInvincible, PlayerData);
MONO_FIELD_OFFSET(scenesVisited, PlayerData);

#undef MONO_ASSEMBLY
#undef MONO_NAMESPACE

#undef MONO_FIELD_OFFSET
#endif