#include "../mono/mono_sdk.h"

#define MONO_GAME_FUNC(r, n, p, s) extern r(*n)p
#include "../game/game_classes.h"
#include "../game/game_functions.h"
#undef MONO_GAME_FUNC

HeroController** HeroController::_instance() {
    static HeroController** ppHeroController =
        (HeroController**)GetStaticField(klass, "Assembly-CSharp", "", "HeroController", "_instance");
    return ppHeroController;
}

PlayerData** HeroController::playerData() {
    static int offset = this->GetFieldOffset(klass, "playerData");
    return (PlayerData**)((char*)this + offset);
}

Transform** HeroController::transform() {
    static int offset = this->GetFieldOffset(klass, "transform");
    return (Transform**)((char*)this + offset);
}

PlayerData** PlayerData::_instance() {
    static PlayerData** ppPlayerData =
        (PlayerData**)GetStaticField(klass, "Assembly-CSharp", "", "PlayerData", "_instance");
    return ppPlayerData;
}

bool* PlayerData::infiniteAirJump() {
    static int offset = this->GetFieldOffset(klass, "infiniteAirJump");
    return (bool*)((char*)this + offset);
}

bool* PlayerData::isInvincible() {
    static int offset = this->GetFieldOffset(klass, "isInvincible");
    return (bool*)((char*)this + offset);
}

List<String*>** PlayerData::scenesVisited() {
    static int offset = this->GetFieldOffset(klass, "scenesVisited");
    return (List<String*>**)((char*)this + offset);
}

Vector3 Transform::get_position() {
    MonoJitInfo* ji = mono_jit_info_table_find(mono_get_root_domain(), Transform_getPosition);
    MonoMethod* method = mono_jit_info_get_method(ji);
    return *(Vector3*)mono_object_unbox(mono_runtime_invoke(method, this, nullptr, nullptr));
}