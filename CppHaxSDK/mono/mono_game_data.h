#pragma once

/*-------------------------------------
Declare ALL your game classes here like

class HeroController;
class PlayerData;
class Transform;
...
-------------------------------------*/

class HeroController;
class PlayerData;
class Transform;

// game data, do not touch
namespace game {
    namespace static_fields {
        #include "mono_game_static_fields.h"
    }
    namespace offsets {
        #include "mono_game_offsets.h"
    }
    namespace funcs {
        #include "mono_game_functions.h"
    }
}


/*-----------------------------------------------------------------
Write your game classes here. 
Don't forget to inherit them from MonoObject
-----------------------------------------------------------------*/

class HeroController : public MonoObject {
public:
    void AddGeo(int amount) { game::funcs::HeroController_AddGeo(this, amount); }
public:
    static HeroController* _instance() { return *game::static_fields::HeroController__instance; }
    PlayerData* playerData() { return *(PlayerData**)((char*)this + game::offsets::HeroController_playerData); }
    Transform* transform() { return *(Transform**)((char*)this + game::offsets::HeroController_transform); }
};

class PlayerData : public MonoObject {
public:
    static PlayerData* _instance() { return *game::static_fields::PlayerData__instance; }
    bool& infiniteAirJump() { return *(bool*)((char*)this + game::offsets::PlayerData_infiniteAirJump); }
    bool& isInvincible() { return *(bool*)((char*)this + game::offsets::PlayerData_isInvincible); }
    List<String*>* scenesVisited() { return *(List<String*>**)((char*)this + game::offsets::PlayerData_scenesVisited); }
};

class Transform : public MonoObject {
public:
    Vector3 get_position() {
        MonoMethod* method = this->GetMonoMethod(game::funcs::Transform_get_Position);
        return *(Vector3*)mono_object_unbox(mono_runtime_invoke(method, this, nullptr, nullptr));
    }
};