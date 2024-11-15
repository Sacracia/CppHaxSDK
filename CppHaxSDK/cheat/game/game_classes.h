#pragma once

#include "../mono/mono_api_classes.h"

class PlayerData;
class Transform;

class HeroController : public MonoObject {
    inline static MonoClass* klass = nullptr;
public:
    static HeroController** _instance();
    PlayerData** playerData();
    Transform** transform();
};

class PlayerData : public MonoObject {
    inline static MonoClass* klass = nullptr;
public:
public:
    static PlayerData** _instance();
    bool* infiniteAirJump();
    bool* isInvincible();
    List<String*>** scenesVisited();
};

class Transform : public MonoObject {
public:
    Vector3 get_position();
};
