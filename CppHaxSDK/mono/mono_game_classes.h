#pragma once

#include "mono_api_classes.h"

class PlayerData;

struct Vector3 {
    float x;
    float y;
    float z;
};

class HeroController : public MonoObject {
    inline static MonoClass* klass = nullptr;
public:
    static HeroController** _instance();
    PlayerData** playerData();
};

class PlayerData : public MonoObject {
    inline static MonoClass* klass = nullptr;
public:
    static PlayerData** _instance();
    bool* infiniteAirJump();
};