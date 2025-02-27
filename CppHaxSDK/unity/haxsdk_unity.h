#pragma once

#include "../haxsdk.h"

namespace Unity {
    struct AsyncOperation;
    struct Behaviour;
    struct BoxCollider;
    struct Camera;
    struct Component;
    struct GameObject;
    struct Light;
    struct Object;
    struct Quaternion;
    struct Screen;
    struct Transform;
    struct Vector2;
    struct Vector3;
}

namespace Unity {
    struct AsyncOperation {
        AsyncOperation() = delete;
        AsyncOperation(const AsyncOperation&) = delete;
    public:
        static ::Class* GetClass();
    public:
        bool GetIsDone();
        float GetProgress();
    };

    struct Behaviour {
        Behaviour() = delete;
        Behaviour(const Behaviour&) = delete;
    public:
        static ::Class* GetClass();
    public:
        bool GetEnabled();
        bool GetIsActiveAndEnabled();
        void SetEnabled(bool value);
    };

    struct BoxCollider {
        BoxCollider() = delete;
        BoxCollider(const BoxCollider&) = delete;
    public:
        static ::Class* GetClass();
    public:
        Vector3 InternalGetCenter();
        Vector3 InternalGetSize();
        void InternalSetCenter(Vector3 value);
        void InternalSetSize(Vector3 value);
        Vector3 GetCenter();
        Vector3 GetSize();
    };

    struct Object : System::Object {
        Object() = delete;
        Object(const Object&) = delete;
    public:
        static ::Class*                           GetClass();
        static System::Array<Object*>*          FindObjectsOfType(System::Type* type);
        static Object*                          FindObjectOfType(System::Type* type);
        static void                             Destroy(Object* obj, float t = 0.f);
    public:
        System::String*                         GetName();
    };

    struct GameObject : Object {
        GameObject() = delete;
        GameObject(const GameObject&) = delete;
    public:
        static ::Class*                           GetClass();
        static inline GameObject*               New()                                       { return (GameObject*)System::Object::New(GetClass())->Ctor(); }
        static GameObject*                      New(const char* name);
        Transform*                              GetTransform();
        void                                    SetLayer(Int32 value);
        bool                                    GetActiveSelf();
        Component*                              GetComponent(System::Type* type);
        Component*                              AddComponent(System::Type* componentType);
        System::Array<Component*>*              GetComponentsInChildren(System::Type* pType, bool includeInactive);
        void                                    SetActive(bool value);
        bool                                    GetActive();
    };

    struct Component : Object {
        Component() = delete;
        Component(const Component&) = delete;
    public:
        static ::Class*                           GetClass();
    public:
        Transform*                              GetTransform();
        GameObject*                             GetGameObject();
    };

    struct Transform : Component {
        static ::Class* GetClass();
        Vector3                                 GetPosition();
        void                                    SetPosition(Unity::Vector3 value);
        Transform*                              GetParent();
        void                                    SetParent(Unity::Transform* value);
        Vector3                                 GetForward();
    };

    struct Vector3 {
        float x, y, z;
    };

    struct Light {
        static ::Class*                           GetClass();
    public:
        void                                    SetIntensity(float value);
        float                                   GetIntensity();
        void                                    SetRange(float value);
        float                                   GetRange();
    };
}