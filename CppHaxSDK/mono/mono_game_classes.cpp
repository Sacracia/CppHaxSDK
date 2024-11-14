#include "mono_game_classes.h"
#include "mono.h"

HeroController** HeroController::_instance() {
    static HeroController** ppHeroController =
        (HeroController**)mono::GetStaticField(klass, "Assembly-CSharp", "", "HeroController", "_instance");
    return ppHeroController;
}

PlayerData** HeroController::playerData() {
    static int offset = mono::GetFieldOffset(klass, this, "playerData");
    return (PlayerData**)((char*)this + offset);
}

PlayerData** PlayerData::_instance() {
    static PlayerData** ppPlayerData =
        (PlayerData**)mono::GetStaticField(klass, "Assembly-CSharp", "", "PlayerData", "_instance");
    return ppPlayerData;
}

bool* PlayerData::infiniteAirJump() {
    static int offset = mono::GetFieldOffset(klass, this, "infiniteAirJump");
    return (bool*)((char*)this + offset);
}

bool* PlayerData::isInvincible() {
    static int offset = mono::GetFieldOffset(klass, this, "isInvincible");
    return (bool*)((char*)this + offset);
}