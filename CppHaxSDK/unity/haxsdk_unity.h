#pragma once

#ifdef HAX_MONO
#include "../backends/haxsdk_mono.h"
#elif defined(HAX_IL2CPP)
#include "../backends/haxsdk_il2cpp.h"
#endif

namespace Unity {
    struct Object;
    struct Vector3;
    struct Transform;
    struct Component;
    struct GameObject;
    struct Camera;
    struct Screen;
    struct Light;
    struct Quaternion;
}

namespace HaxSdk {
    void InitializeUnityData();
}

struct Unity::Quaternion {
    static Unity::Quaternion                GetIdentity() { return { 0.F, 0.F, 0.F, 1.F }; }
public:
    float x;
    float y;
    float z;
    float w;
};

struct Unity::Vector3 {
    static float                            Distance(Unity::Vector3& a, Unity::Vector3& b);
    float                                   Distance(Unity::Vector3& other);
    float                                   Distance(Unity::Vector3&& other);
    Vector3                                 operator+(const Unity::Vector3& a) const;
    Vector3                                 operator*(float mult) const;
public:
    float x;
    float y;
    float z;
};

struct Unity::Object : System::Object {
    static System::Array<Unity::Object*>*   FindObjectsOfType(System::Type* type);
    static Unity::Object*                   FindObjectOfType(System::Type* type);
    static void                             Destroy(Unity::Object* obj);
public:
    System::String*                         GetName();
};

struct Unity::Component : Unity::Object {
    Unity::Transform*                       GetTransform();
    Unity::GameObject*                      GetGameObject();
};

struct Unity::Transform : Unity::Component {
    Unity::Vector3                          GetPosition();
    void                                    SetPosition(Unity::Vector3 value);
    Unity::Transform*                       GetParent();
    void                                    SetParent(Unity::Transform* value);
    Unity::Vector3                          GetForward();
};

struct Unity::GameObject : Unity::Object {
    static Unity::GameObject*               Ctor();
    static Unity::GameObject*               Ctor(const char* name);
    Unity::Transform*                       GetTransform();
    void                                    SetLayer(int32_t value);
    bool                                    GetActiveSelf();
    Unity::Component*                       GetComponent(System::Type* type);
    Unity::Component*                       AddComponent(System::Type* componentType);
    void                                    SetActive(bool value);
};

struct Unity::Camera {
    static Unity::Camera*                   GetMain();
    Unity::Vector3                          WorldToScreenPoint(Unity::Vector3 position);
};

struct Unity::Screen {
    static int                              GetWidth();
    static int                              GetHeight();
};

struct Unity::Light {
    void                                    SetIntensity(float value);
    float                                   GetIntensity();
    void                                    SetRange(float value);
    float                                   GetRange();
};