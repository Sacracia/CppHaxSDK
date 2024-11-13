#include "logger.h"
#include "utils.h"

STATIC_FIELD(HeroController, HeroController*, _instance, "Assembly-CSharp", "")
FIELD(HeroController, PlayerData*, playerData, "Assembly-CSharp", "")

STATIC_FIELD(PlayerData, PlayerData*, _instance, "Assembly-CSharp", "")
FIELD(PlayerData, bool, infiniteAirJump, "Assembly-CSharp", "")