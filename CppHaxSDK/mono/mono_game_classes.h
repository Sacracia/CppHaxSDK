#pragma once

#include <stdint.h>

#include "mono_api_classes.h"

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
    List<String>** scenesVisited();
};
