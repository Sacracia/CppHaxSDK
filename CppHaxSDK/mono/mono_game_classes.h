#pragma once

#include "mono_api_classes.h"

struct Vector3 {
    float x;
    float y;
    float z;
};

class PlayerData;

class HeroController : public MonoObject {
    inline static MonoClass* klass = nullptr;
public:
    static HeroController** _instance();
public:
    PlayerData** playerData();
};

class PlayerData : public MonoObject {
    inline static MonoClass* klass = nullptr;
public:
    static PlayerData** _instance();
public:
    bool* infiniteAirJump();
    bool* isInvincible();
};