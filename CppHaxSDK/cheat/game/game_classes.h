#pragma once

class HeroController;
class PlayerData;
class Transform;

class HeroController : public MonoObject {
public:
    void AddGeo(int amount) { hero_controller::funcs::HeroController_AddGeo(this, amount); }
public:
    static HeroController* _instance() { return *(HeroController**)hero_controller::static_fields::_instance; }
    PlayerData* playerData() { return *(PlayerData**)((char*)this + hero_controller::offsets::playerData); }
    Transform* transform() { return *(Transform**)((char*)this + hero_controller::offsets::transform); }
};

class PlayerData : public MonoObject {
public:
    static PlayerData* _instance() { return *(PlayerData**)player_data::static_fields::_instance; }
    bool& infiniteAirJump() { return *(bool*)((char*)this + player_data::offsets::infiniteAirJump); }
    bool& isInvincible() { return *(bool*)((char*)this + player_data::offsets::isInvincible); }
    List<String*>* scenesVisited() { return *(List<String*>**)((char*)this + player_data::offsets::scenesVisited); }
};

class Transform : public MonoObject {
public:
    Vector3 get_position() { 
        MonoMethod* method = this->GetMonoMethod(transform::funcs::Transform_getPosition);
        return *(Vector3*)mono_object_unbox(mono_runtime_invoke(method, this, nullptr, nullptr)); 
    }
};
