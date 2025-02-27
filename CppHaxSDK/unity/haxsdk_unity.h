#pragma once

#include "../haxsdk.h"

namespace Unity {
    struct AsyncOperation;
    struct Behaviour;
    struct BoxCollider;
    struct Camera;
    struct Component;
    struct Collider;
    struct Collider2D;
    struct GameObject;
    struct KeyCode;
    struct Light;
    struct Object;
    struct Quaternion;
    struct Screen;
    struct Transform;
    struct Vector2;
    struct Vector3;
}

namespace Unity {
    struct Object : System::Object {
        Object() = delete;
        Object(const Object&) = delete;
    public:
        static ::Class*                         GetClass();
        static System::Array<Object*>*          FindObjectsOfType(System::Type* type);
        static Object*                          FindObjectOfType(System::Type* type);
        static void                             Destroy(Object* obj, float t = 0.f);
    public:
        System::String*                         GetName();
    };

    struct Component : Object{
        Component() = delete;
        Component(const Component&) = delete;
    public:
        static ::Class*                         GetClass();
    public:
        Transform*                              GetTransform();
        GameObject*                             GetGameObject();
    };

    struct AsyncOperation {
        AsyncOperation() = delete;
        AsyncOperation(const AsyncOperation&) = delete;
    public:
        static ::Class*                         GetClass();
    public:
        bool                                    GetIsDone();
        float                                   GetProgress();
    };

    struct Behaviour : Component {
        Behaviour() = delete;
        Behaviour(const Behaviour&) = delete;
    public:
        static ::Class*                         GetClass();
    public:
        bool                                    GetEnabled();
        bool                                    GetIsActiveAndEnabled();
        void                                    SetEnabled(bool value);
    };

    struct Collider : Component {
        Collider() = delete;
        Collider(const Collider&) = delete;
    public:
        static ::Class*                         GetClass();
    public:
        bool                                    GetEnabled();
        void                                    SetEnabled(bool value);
    };

    struct BoxCollider : Collider {
        BoxCollider() = delete;
        BoxCollider(const BoxCollider&) = delete;
    public:
        static ::Class*                         GetClass();
    public:
        Vector3                                 InternalGetCenter();
        Vector3                                 InternalGetSize();
        void                                    InternalSetCenter(Vector3 value);
        void                                    InternalSetSize(Vector3 value);
        Vector3                                 GetCenter();
        Vector3                                 GetSize();
    };

    struct Camera : Behaviour {
        enum MonoOrStereoscopicEye : Int32 {
            Left,
            Right,
            Mono
        };
    public:
        Camera() = delete;
        Camera(const Camera&) = delete;
    public:
        static ::Class*                         GetClass();
        static Camera*                          GetMain();
    public:
        Vector3                                 WorldToScreenPoint(Vector3 position);
        Vector3                                 WorldToScreenPoint_Injected(Vector3 position, MonoOrStereoscopicEye eye);
        Vector3                                 INTERNAL_CALL_WorldToScreenPoint(Vector3 position);
    };

    struct Collider2D : Behaviour {
        Collider2D() = delete;
        Collider2D(const Collider2D&) = delete;
    };

    struct GameObject : Object {
        GameObject() = delete;
        GameObject(const GameObject&) = delete;
    public:
        static ::Class*                         GetClass();
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

    struct KeyCode {
        static System::Type*                    GetSystemType();
    };

    struct Transform : Component {
        Transform() = delete;
        Transform(const Transform&) = delete;
    public:
        static ::Class*                         GetClass();
    public:
        Vector3                                 GetPosition();
        void                                    SetPosition(Unity::Vector3 value);
        Transform*                              GetParent();
        void                                    SetParent(Unity::Transform* value);
        Vector3                                 GetForward();
    };

    struct Vector3 {
        float x;
        float y;
        float z;
    };

    struct Light : Behaviour {
        static ::Class*                         GetClass();
    public:
        void                                    SetIntensity(float value);
        float                                   GetIntensity();
        void                                    SetRange(float value);
        float                                   GetRange();
    };
}