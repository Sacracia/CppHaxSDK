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
MONO_FIELD_OFFSET(DASH_SPEED, HeroController);

// PlayerData
MONO_FIELD_OFFSET(infiniteAirJump, PlayerData);
MONO_FIELD_OFFSET(isInvincible, PlayerData);
MONO_FIELD_OFFSET(maxMP, PlayerData);
MONO_FIELD_OFFSET(MPCharge, PlayerData);
MONO_FIELD_OFFSET(scenesVisited, PlayerData);
MONO_FIELD_OFFSET(hasMap, PlayerData);
MONO_FIELD_OFFSET(openedMapperShop, PlayerData);
MONO_FIELD_OFFSET(hasQuill, PlayerData);
MONO_FIELD_OFFSET(gotCharm_2, PlayerData);
MONO_FIELD_OFFSET(mapDirtmouth, PlayerData);
MONO_FIELD_OFFSET(mapCrossroads, PlayerData);
MONO_FIELD_OFFSET(mapGreenpath, PlayerData);
MONO_FIELD_OFFSET(mapFogCanyon, PlayerData);
MONO_FIELD_OFFSET(mapRoyalGardens, PlayerData);
MONO_FIELD_OFFSET(mapFungalWastes, PlayerData);
MONO_FIELD_OFFSET(mapCity, PlayerData);
MONO_FIELD_OFFSET(mapWaterways, PlayerData);
MONO_FIELD_OFFSET(mapMines, PlayerData);
MONO_FIELD_OFFSET(mapDeepnest, PlayerData);
MONO_FIELD_OFFSET(mapCliffs, PlayerData);
MONO_FIELD_OFFSET(mapOutskirts, PlayerData);
MONO_FIELD_OFFSET(mapRestingGrounds, PlayerData);
MONO_FIELD_OFFSET(mapAbyss, PlayerData);
MONO_FIELD_OFFSET(hasPin, PlayerData);
MONO_FIELD_OFFSET(hasPinBench, PlayerData);
MONO_FIELD_OFFSET(hasPinCocoon, PlayerData);
MONO_FIELD_OFFSET(hasPinDreamPlant, PlayerData);
MONO_FIELD_OFFSET(hasPinGuardian, PlayerData);
MONO_FIELD_OFFSET(hasPinBlackEgg, PlayerData);
MONO_FIELD_OFFSET(hasPinShop, PlayerData);
MONO_FIELD_OFFSET(hasPinSpa, PlayerData);
MONO_FIELD_OFFSET(hasPinStag, PlayerData);
MONO_FIELD_OFFSET(hasPinTram, PlayerData);
MONO_FIELD_OFFSET(hasPinGhost, PlayerData);
MONO_FIELD_OFFSET(hasPinGrub, PlayerData);

MONO_FIELD_OFFSET(hasSpell, PlayerData);
MONO_FIELD_OFFSET(fireballLevel, PlayerData);
MONO_FIELD_OFFSET(quakeLevel, PlayerData);
MONO_FIELD_OFFSET(screamLevel, PlayerData);


#undef MONO_ASSEMBLY
#undef MONO_NAMESPACE

#undef MONO_FIELD_OFFSET
#endif