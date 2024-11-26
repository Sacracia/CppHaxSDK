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
class Camera;
class GameCameras;

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
    HeroController();
public:
    static HeroController* _instance() { return *game::static_fields::HeroController__instance; }
    PlayerData* playerData() { return *(PlayerData**)((char*)this + game::offsets::HeroController_playerData); }
    Transform* transform() { return *(Transform**)((char*)this + game::offsets::HeroController_transform); }
};

class PlayerData : public MonoObject {
public:
    static PlayerData* _instance() { return *game::static_fields::PlayerData__instance; }
};

class GameCameras : public MonoObject {
public:
    static GameCameras* _instance() { return *game::static_fields::GameCameras__instance; }
    Camera* mainCamera() { return *(Camera**)((char*)this + game::offsets::GameCameras_mainCamera); }
};

class Transform : public MonoObject {
public:
    Vector3 get_position() {
        MonoMethod* method = this->GetMonoMethod(game::funcs::Transform_get_position);
        return *(Vector3*)mono_object_unbox(mono_runtime_invoke(method, this, nullptr, nullptr));
    }
};

class Camera : public MonoObject {
public:
    float get_fieldOfView() { return game::funcs::Camera_get_fieldOfView(this); }
    void set_fieldOfView(float value) { game::funcs::Camera_set_fieldOfView(this, value); }
    Vector3 WorldToScreenPoint(Vector3 position) {
        MonoMethod* method = this->GetMonoMethod(game::funcs::Camera_WorldToScreenPoint);
        void* args[1] = { &position };
        return *(Vector3*)mono_object_unbox(mono_runtime_invoke(method, this, args, nullptr));
    }
};