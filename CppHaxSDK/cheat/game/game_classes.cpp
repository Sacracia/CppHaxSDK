//#include "../mono/mono_sdk.h"
//
//#include "../game/game_classes.h"
//
//HeroController* HeroController::_instance() {
//    return 
//}
//
//PlayerData* HeroController::playerData() {
//    static int offset = this->GetFieldOffset(klass, "playerData");
//    return *(PlayerData**)((char*)this + offset);
//}
//
//Transform* HeroController::transform() {
//    static int offset = this->GetFieldOffset(klass, "transform");
//    return *(Transform**)((char*)this + offset);
//}
//
//PlayerData* PlayerData::_instance() {
//    static PlayerData** ppPlayerData =
//        (PlayerData**)GetStaticField(klass, "Assembly-CSharp", "", "PlayerData", "_instance");
//    return *ppPlayerData;
//}
//
//bool& PlayerData::infiniteAirJump() {
//    static int offset = this->GetFieldOffset(klass, "infiniteAirJump");
//    return *(bool*)((char*)this + offset);
//}
//
//bool& PlayerData::isInvincible() {
//    static int offset = this->GetFieldOffset(klass, "isInvincible");
//    return *(bool*)((char*)this + offset);
//}
//
//List<String*>* PlayerData::scenesVisited() {
//    static int offset = this->GetFieldOffset(klass, "scenesVisited");
//    return *(List<String*>**)((char*)this + offset);
//}
//
//Vector3 Transform::get_position() {
//    static MonoMethod* method = this->GetMonoMethod(Transform_getPosition);
//    return *(Vector3*)mono_object_unbox(mono_runtime_invoke(method, this, nullptr, nullptr));
//}