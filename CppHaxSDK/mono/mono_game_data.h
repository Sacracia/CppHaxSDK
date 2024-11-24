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
private:
    HeroController();
public:
    void AddGeo(int amount) { 
        game::funcs::HeroController_AddGeo(this, amount); 
    }
    void AddHealth(int amount) { 
        game::funcs::HeroController_AddHealth(this, amount); 
    }
    void AddMPCharge(int amount) { 
        game::funcs::HeroController_AddMPCharge(this, amount); 
    }
public:
    static HeroController* _instance() { 
        return *game::static_fields::HeroController__instance; 
    }
    PlayerData* playerData() { 
        return *(PlayerData**)((char*)this + game::offsets::HeroController_playerData); 
    }
    Transform* transform() { 
        return *(Transform**)((char*)this + game::offsets::HeroController_transform); 
    }
    float& DASH_SPEED() { 
        return *(float*)((char*)this + game::offsets::HeroController_DASH_SPEED); 
    }
};

class PlayerData : public MonoObject {
public:
    static PlayerData* _instance() { return *game::static_fields::PlayerData__instance; }
    int& maxHealth() { return *(int*)((char*)this + game::offsets::PlayerData_maxHealth); }
    int& health() { return *(int*)((char*)this + game::offsets::PlayerData_health); }
    bool& infiniteAirJump() { return *(bool*)((char*)this + game::offsets::PlayerData_infiniteAirJump); }
    bool& isInvincible() { return *(bool*)((char*)this + game::offsets::PlayerData_isInvincible); }
    int& maxMP() { return *(int*)((char*)this + game::offsets::PlayerData_maxMP); }
    int& MPCharge() { return *(int*)((char*)this + game::offsets::PlayerData_MPCharge); }
    List<String*>* scenesVisited() { return *(List<String*>**)((char*)this + game::offsets::PlayerData_scenesVisited); }
    bool& hasMap() { return *(bool*)((char*)this + game::offsets::PlayerData_hasMap); }
    bool& openedMapperShop() { return *(bool*)((char*)this + game::offsets::PlayerData_openedMapperShop); }
    bool& hasQuill() { return *(bool*)((char*)this + game::offsets::PlayerData_hasQuill); }
    bool& gotCharm_2() { return *(bool*)((char*)this + game::offsets::PlayerData_gotCharm_2); }
    bool& mapDirtmouth() { return *(bool*)((char*)this + game::offsets::PlayerData_mapDirtmouth); }
    bool& mapCrossroads() { return *(bool*)((char*)this + game::offsets::PlayerData_mapCrossroads); }
    bool& mapGreenpath() { return *(bool*)((char*)this + game::offsets::PlayerData_mapGreenpath); }
    bool& mapFogCanyon() { return *(bool*)((char*)this + game::offsets::PlayerData_mapFogCanyon); }
    bool& mapRoyalGardens() { return *(bool*)((char*)this + game::offsets::PlayerData_mapRoyalGardens); }
    bool& mapFungalWastes() { return *(bool*)((char*)this + game::offsets::PlayerData_mapFungalWastes); }
    bool& mapCity() { return *(bool*)((char*)this + game::offsets::PlayerData_mapCity); }
    bool& mapWaterways() { return *(bool*)((char*)this + game::offsets::PlayerData_mapWaterways); }
    bool& mapMines() { return *(bool*)((char*)this + game::offsets::PlayerData_mapMines); }
    bool& mapDeepnest() { return *(bool*)((char*)this + game::offsets::PlayerData_mapDeepnest); }
    bool& mapCliffs() { return *(bool*)((char*)this + game::offsets::PlayerData_mapCliffs); }
    bool& mapOutskirts() { return *(bool*)((char*)this + game::offsets::PlayerData_mapOutskirts); }
    bool& mapRestingGrounds() { return *(bool*)((char*)this + game::offsets::PlayerData_mapRestingGrounds); }
    bool& mapAbyss() { return *(bool*)((char*)this + game::offsets::PlayerData_mapAbyss); }
    bool& hasPin() { return *(bool*)((char*)this + game::offsets::PlayerData_hasPin); }
    bool& hasPinBench() { return *(bool*)((char*)this + game::offsets::PlayerData_hasPinBench); }
    bool& hasPinCocoon() { return *(bool*)((char*)this + game::offsets::PlayerData_hasPinCocoon); }
    bool& hasPinDreamPlant() { return *(bool*)((char*)this + game::offsets::PlayerData_hasPinDreamPlant); }
    bool& hasPinGuardian() { return *(bool*)((char*)this + game::offsets::PlayerData_hasPinGuardian); }
    bool& hasPinBlackEgg() { return *(bool*)((char*)this + game::offsets::PlayerData_hasPinBlackEgg); }
    bool& hasPinShop() { return *(bool*)((char*)this + game::offsets::PlayerData_hasPinShop); }
    bool& hasPinSpa() { return *(bool*)((char*)this + game::offsets::PlayerData_hasPinSpa); }
    bool& hasPinStag() { return *(bool*)((char*)this + game::offsets::PlayerData_hasPinStag); }
    bool& hasPinTram() { return *(bool*)((char*)this + game::offsets::PlayerData_hasPinTram); }
    bool& hasPinGhost() { return *(bool*)((char*)this + game::offsets::PlayerData_hasPinGhost); }
    bool& hasPinGrub() { return *(bool*)((char*)this + game::offsets::PlayerData_hasPinGrub); }

    bool& hasSpell() { return *(bool*)((char*)this + game::offsets::PlayerData_hasSpell); }
    int& fireballLevel() { return *(int*)((char*)this + game::offsets::PlayerData_fireballLevel); }
    int& quakeLevel() { return *(int*)((char*)this + game::offsets::PlayerData_quakeLevel); }
    int& screamLevel() { return *(int*)((char*)this + game::offsets::PlayerData_screamLevel); }

    bool& hasAcidArmour() { return *(bool*)((char*)this + game::offsets::PlayerData_hasAcidArmour); }
    bool& hasDreamNail() { return *(bool*)((char*)this + game::offsets::PlayerData_hasDreamNail); }
    int& dreamOrbs() { return *(int*)((char*)this + game::offsets::PlayerData_dreamOrbs); }
    bool& dreamNailUpgraded() { return *(bool*)((char*)this + game::offsets::PlayerData_dreamNailUpgraded); }
    bool& hasDreamGate() { return *(bool*)((char*)this + game::offsets::PlayerData_hasDreamGate); }
    // keys
    bool& hasCityKey() { return *(bool*)((char*)this + game::offsets::PlayerData_hasCityKey); }
    bool& hasWhiteKey() { return *(bool*)((char*)this + game::offsets::PlayerData_hasWhiteKey); }
    bool& hasKingsBrand() { return *(bool*)((char*)this + game::offsets::PlayerData_hasKingsBrand); }
    bool& hasLoveKey() { return *(bool*)((char*)this + game::offsets::PlayerData_hasLoveKey); }
    bool& hasSlykey() { return *(bool*)((char*)this + game::offsets::PlayerData_hasSlykey); }
    int& simpleKeys() { return *(int*)((char*)this + game::offsets::PlayerData_simpleKeys); }

    bool& hasTramPass() { return *(bool*)((char*)this + game::offsets::PlayerData_hasTramPass); }
    bool& hasLantern() { return *(bool*)((char*)this + game::offsets::PlayerData_hasLantern); }
    bool& hasJournal() { return *(bool*)((char*)this + game::offsets::PlayerData_hasJournal); }
    bool& hasHuntersMark() { return *(bool*)((char*)this + game::offsets::PlayerData_hasHuntersMark); }
    bool& hasXunFlower() { return *(bool*)((char*)this + game::offsets::PlayerData_hasXunFlower); }
    bool& hasGodfinder() { return *(bool*)((char*)this + game::offsets::PlayerData_hasGodfinder); }

    bool& hasCharm() { return *(bool*)((char*)this + game::offsets::PlayerData_hasCharm); }
    bool& gotCharm_1() { return *(bool*)((char*)this + game::offsets::PlayerData_gotCharm_1); }
    bool& gotCharm_40() { return *(bool*)((char*)this + game::offsets::PlayerData_gotCharm_40); }
    
    bool& hasDash() { return *(bool*)((char*)this + game::offsets::PlayerData_hasDash); }
    bool& canDash() { return *(bool*)((char*)this + game::offsets::PlayerData_canDash); }
    bool& hasShadowDash() { return *(bool*)((char*)this + game::offsets::PlayerData_hasShadowDash); }
    bool& canShadowDash() { return *(bool*)((char*)this + game::offsets::PlayerData_canShadowDash); }
};

class Transform : public MonoObject {
public:
    Vector3 get_position() {
        MonoMethod* method = this->GetMonoMethod(game::funcs::Transform_get_Position);
        return *(Vector3*)mono_object_unbox(mono_runtime_invoke(method, this, nullptr, nullptr));
    }
};