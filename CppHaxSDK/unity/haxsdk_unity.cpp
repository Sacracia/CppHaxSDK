#include "haxsdk_unity.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>

#define HAXSDK_FUNCTION(a, n, c, m, s)          static Method c ## __ ## m
#define HAXSDK_FUNCTION_EXT(a, n, c, m, mn, s)  static Method c ## __ ## m
#define HAXSDK_STATIC_FIELD(a, n, c, f, t)      static t* c ## __ ## f
#define HAXSDK_FIELD_OFFSET(a, n, c, f)         static int c ## __ ## f
#include "../unity/haxsdk_unity_data.h"

static const char* UNITY_CORE_ASSEMBLY = nullptr;

void HaxSdk::InitializeUnityData() {
    UNITY_CORE_ASSEMBLY = Domain::TryFindAssembly("UnityEngine.CoreModule") ? "UnityEngine.CoreModule" : "UnityEngine";
    #define HAXSDK_FUNCTION(a, n, c, m, s)          c ## __ ## m = Class::Find(a, n, #c)->FindMethod(#m, s)
    #define HAXSDK_FUNCTION_EXT(a, n, c, m, mn, s)  c ## __ ## m = Class::Find(a, n, #c)->FindMethod(mn, s)
    #define HAXSDK_STATIC_FIELD(a, n, c, f, t)      c ## __ ## f = (t*)Class::Find(a, n, #c)->FindStaticField(#f)
    #define HAXSDK_FIELD_OFFSET(a, n, c, f)         c ## __ ## f = Class::Find(a, n, #c)->FindField(#f)->Offset()
    #include "haxsdk_unity_data.h" 
}

float Unity::Vector3::Distance(Unity::Vector3& a, Unity::Vector3& b) {
    Vector3 vector = { a.x - b.x, a.y - b.y, a.z - b.z };
    return std::sqrt(vector.x * vector.x + vector.y + vector.y + vector.z + vector.z);
}

float Unity::Vector3::Distance(Unity::Vector3& other) {
    Vector3 vector = { this->x - other.x, this->y - other.y, this->z - other.z };
    return std::sqrt(vector.x * vector.x + vector.y + vector.y + vector.z + vector.z);
}

float Unity::Vector3::Distance(Unity::Vector3&& other) {
    Vector3 vector = { this->x - other.x, this->y - other.y, this->z - other.z };
    return std::sqrt(vector.x * vector.x + vector.y + vector.y + vector.z + vector.z);
}

Unity::Vector3 Unity::Vector3::operator+(const Unity::Vector3& a) const {
    return { x + a.x, y + a.y, z + a.z };
}

Unity::Vector3 Unity::Vector3::operator*(float mult) const {
    return { x * mult, y * mult, z * mult };
}

System::Array<Unity::Object*>* Unity::Object::FindObjectsOfType(System::Type* type) {
    return reinterpret_cast<System::Array<Unity::Object*>*(*)(System::Type*)>(Object__FindObjectsOfType.Ptr())(type);
}

Unity::Object* Unity::Object::FindObjectOfType(System::Type* type) {
    return reinterpret_cast<Unity::Object*(*)(System::Type*)>(Object__FindObjectOfType.Ptr())(type);
}

void Unity::Object::Destroy(Unity::Object* obj) { 
    reinterpret_cast<void(*)(Unity::Object*,float)>(Object__Destroy.Ptr())(obj,0.F);
}

System::String* Unity::Object::GetName() {
    return reinterpret_cast<System::String*(*)(Unity::Object*)>(Object__get_name.Ptr())(this);
}

Unity::Vector3 Unity::Transform::GetPosition() {
#ifdef HAX_MONO
    return *(Unity::Vector3*)Transform__get_position.Invoke(this, nullptr)->Unbox();
#else
    return reinterpret_cast<Unity::Vector3(*)(Unity::Transform*)>(Transform__get_position.Ptr())(this);
#endif
}

void Unity::Transform::SetPosition(Unity::Vector3 value) {
#ifdef HAX_MONO
    void* args[1] = {&value};
    Transform__set_position.Invoke(this, args);
#else
    reinterpret_cast<void(*)(Unity::Transform*,Unity::Vector3)>(Transform__set_position.Ptr())(this, value);
#endif
}

Unity::Transform* Unity::Transform::GetParent() {
    return reinterpret_cast<Unity::Transform*(*)(Unity::Transform*)>(Transform__get_parent.Ptr())(this);
}

Unity::Vector3 Unity::Transform::GetForward() {
    return *(Unity::Vector3*)Transform__get_forward.Invoke(this, nullptr)->Unbox();
}

void Unity::Transform::SetParent(Unity::Transform* value) {
    return reinterpret_cast<void(*)(Transform*, Transform*)>(Transform__set_parent.Ptr())(this, value);
}

Unity::Transform* Unity::Component::GetTransform() {
    return reinterpret_cast<Unity::Transform*(*)(Unity::Component*)>(Component__get_transform.Ptr())(this);
}

Unity::GameObject* Unity::Component::GetGameObject() {
    return reinterpret_cast<Unity::GameObject*(*)(Unity::Component*)>(Component__get_gameObject.Ptr())(this);
}

Unity::Transform* Unity::GameObject::GetTransform() {
    return reinterpret_cast<Unity::Transform*(*)(Unity::GameObject*)>(GameObject__get_transform.Ptr())(this);
}

void Unity::GameObject::SetLayer(int32_t value) {
    return reinterpret_cast<void(*)(Unity::GameObject*,int32_t)>(GameObject__set_layer.Ptr())(this, value);
}

bool Unity::GameObject::GetActiveSelf() {
    return reinterpret_cast<bool(*)(Unity::GameObject*)>(GameObject__get_activeSelf.Ptr())(this);
}

Unity::GameObject* Unity::GameObject::Ctor() {
    static Class* pGameObjectClass = Class::Find(UNITY_CORE_ASSEMBLY, "UnityEngine", "GameObject");
    return (Unity::GameObject*)System::Object::NewI(pGameObjectClass);
}

Unity::GameObject* Unity::GameObject::Ctor(const char* name) {
    static Class* pGameObjectClass = Class::Find(UNITY_CORE_ASSEMBLY, "UnityEngine", "GameObject");
    Unity::GameObject* pObj = (GameObject*)Unity::GameObject::New(pGameObjectClass);
    void* args[1] = { System::String::New(name) };
    GameObject__ctor.Invoke(pObj, args);
    return pObj;
}

Unity::Component* Unity::GameObject::AddComponent(System::Type* componentType) {
    return reinterpret_cast<Component*(*)(GameObject*,System::Type*)>(GameObject__AddComponent.Ptr())(this, componentType);
}

Unity::Component* Unity::GameObject::GetComponent(System::Type* type) {
    return reinterpret_cast<Unity::Component*(*)(Unity::GameObject*,System::Type*)>(GameObject__GetComponent.Ptr())(this, type);
}

void Unity::GameObject::SetActive(bool value) {
    void* args[1] = {&value};
    GameObject__SetActive.Invoke(this, args);
}

Unity::Camera* Unity::Camera::GetMain() {
    return reinterpret_cast<Unity::Camera*(*)()>(Camera__get_main.Ptr())();
}

Unity::Vector3 Unity::Camera::WorldToScreenPoint(Unity::Vector3 position) {
#ifdef HAX_MONO
    void* args[1] = { &position };
    return *(Unity::Vector3*)Camera__WorldToScreenPoint.Invoke(this, args)->Unbox();
#else
    reinterpret_cast<Unity::Vector3(*)(Unity::Camera*,Unity::Vector3)>(Camera__WorldToScreenPoint.Ptr())(this, position);
#endif
}

int Unity::Screen::GetWidth() {
    return reinterpret_cast<int(*)()>(Screen__get_width.Ptr())();
}

int Unity::Screen::GetHeight() {
    return reinterpret_cast<int(*)()>(Screen__get_height.Ptr())();
}

float Unity::Light::GetIntensity() {
    return reinterpret_cast<float(*)(Unity::Light*)>(Light__get_intensity.Ptr())(this);
}

void Unity::Light::SetIntensity(float value) {
    reinterpret_cast<void(*)(Unity::Light*,float)>(Light__set_intensity.Ptr())(this, value);
}

float Unity::Light::GetRange() {
    return reinterpret_cast<float(*)(Unity::Light*)>(Light__get_range.Ptr())(this);
}

void Unity::Light::SetRange(float value) {
    return reinterpret_cast<void(*)(Unity::Light*,float)>(Light__set_range.Ptr())(this, value);
}