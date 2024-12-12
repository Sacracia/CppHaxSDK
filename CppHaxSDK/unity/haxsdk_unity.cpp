#include "haxsdk_unity.h"

#include <iostream>

#ifdef _WIN64
#define UNITY_CORE_ASSEMBLY "UnityEngine.CoreModule"
#else
#define UNITY_CORE_ASSEMBLY "UnityEngine"
#endif

#define HAXSDK_FUNCTION(a, n, c, m, s)     static BackendMethod* c ## __ ## m
#define HAXSDK_STATIC_FIELD(a, n, c, f, t) static t* c ## __ ## f
#define HAXSDK_FIELD_OFFSET(a, n, c, f)    static int c ## __ ## f
#include "../unity/haxsdk_unity_data.h"

void HaxSdk::InitializeUnityData() {
    #define HAXSDK_FUNCTION(a, n, c, m, s)     c ## __ ## m = BackendClass::find(a, n, #c)->find_method(#m, s)
    #define HAXSDK_STATIC_FIELD(a, n, c, f, t) c ## __ ## f = (t*)BackendClass::find(a, n, #c)->find_static_field(#f)
    #define HAXSDK_FIELD_OFFSET(a, n, c, f)    c ## __ ## f = BackendClass::find(a, n, #c)->find_field(#f)->offset()
    #include "haxsdk_unity_data.h"
}

float Vector3::Distance(Vector3 a, Vector3 b) {
    return reinterpret_cast<float(*)(Vector3, Vector3)>(Vector3__Distance->address())(a, b);
}

Array<BackendObject*>* Object::FindObjectsOfType(SystemType* type) {
    return reinterpret_cast<Array<BackendObject*>*(*)(SystemType*)>(Object__FindObjectsOfType->address())(type);
}

BackendObject* Object::FindObjectOfType(SystemType* type) {
    return reinterpret_cast<BackendObject*(*)(SystemType*)>(Object__FindObjectOfType->address())(type);
}

Vector3 Transform::get_position() {
    return *(Vector3*)Transform__get_position->invoke(this, nullptr)->unbox();
}

void Transform::set_position(Vector3 value) {
    reinterpret_cast<void(*)(Transform*, Vector3)>(Transform__set_position->address())(this, value);
}

Transform* Component::get_transform() {
    return reinterpret_cast<Transform*(*)(Component*)>(Component__get_transform->address())(this);
}

Camera* Camera::main() {
    return reinterpret_cast<Camera*(*)()>(Camera__get_main->address())();
}

Vector3 Camera::WorldToScreenPoint(Vector3 position) {
    return reinterpret_cast<Vector3(*)(Camera*,Vector3)>(Camera__WorldToScreenPoint->address())(this, position);
}

int Screen::width() {
    return reinterpret_cast<int(*)()>(Screen__get_width->address())();
}

int Screen::height() {
    return reinterpret_cast<int(*)()>(Screen__get_height->address())();
}