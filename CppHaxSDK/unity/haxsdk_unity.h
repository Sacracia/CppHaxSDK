#pragma once

#include "../system/haxsdk_system.h"

namespace HaxSdk
{
    void InitUnity();
}

namespace Unity {
    struct                                      AsyncOperation;
    struct                                      Behaviour;
    struct                                      BoxCollider;
    struct                                      Bounds;
    struct                                      Camera;
    struct                                      Component;
    struct                                      Collider;
    struct                                      Collider2D;
    struct                                      Color;
    struct                                      GameObject;
    struct                                      KeyCode;
    struct                                      Light;
    struct                                      LineRenderer;
    struct                                      Object;
    struct                                      Renderer;
    struct                                      Rect;
    struct                                      Screen;
    struct                                      Sprite;
    struct                                      Shader;
    struct                                      Material;
    struct                                      Transform;
    struct                                      Vector2;
    struct                                      Vector3;
    struct                                      Vector3_Boxed;
    struct                                      Quaternion;
}

namespace Unity
{
    struct Transform;
    struct Vector3;
    struct GameObject;

    enum HideFlags : Int32 {
        None = 0,
        HideInHierarchy = 1,
        HideInInspector = 2,
        DontSaveInEditor = 4,
        NotEditable = 8,
        DontSaveInBuild = 16,
        DontUnloadUnusedAsset = 32,
        DontSave = 52,
        HideAndDontSave = 61
    };

    struct Object : System::Object
    {
        explicit Object(unsafe::Object* ptr) : System::Object(ptr) {}

        inline operator bool() const { return !Null(); }

        inline bool Null() const { return !m_Pointer || !GetPointer()->m_CachedPtr; }

        inline unsafe::Unity::Object* GetPointer() const { return (unsafe::Unity::Object*)m_Pointer; }
    
        static Object Instantiate(const Object& original);
        static Object Instantiate(const Object& original, const Vector3& position, const Quaternion& rotation);;
        static System::Array<Object> FindObjectsOfType(System::Type type);
        static Object FindObjectOfType(System::Type type);
        static void                             Destroy(Object obj, float t = 0.f);

        System::String GetName();
        void                                    SetName(System::String pName);
        void                                    SetHideFlags(HideFlags flags);
    };

    struct Component : Object
    {
        explicit Component(unsafe::Object* ptr) : Object(ptr) {}

        inline operator bool() const { return !Null(); }

        Transform GetTransform();
        GameObject GetGameObject();

        Component GetComponentInChildren(System::Type);
        System::Array<Component> GetComponentsInChildren(System::Type);
        Component GetComponent(System::Type);
    };

    struct Transform : Component
    {
        explicit Transform(unsafe::Object* ptr) : Component(ptr) {}

        inline operator bool() const { return !Null(); }

        Vector3                                 GetPosition();
        void                                    SetPosition(const Unity::Vector3& value);
        Transform                               GetParent();
        void                                    SetParent(const Unity::Transform& value);
        Vector3                                 GetForward();
    };

    struct Behaviour : Component
    {
        explicit Behaviour(unsafe::Object* ptr) : Component(ptr) {}

        inline operator bool() const { return !Null(); }
    };

    struct Camera : Behaviour
    {
        explicit Camera(unsafe::Object* ptr) : Behaviour(ptr) {}

        inline operator bool() const { return !Null(); }

        static Camera GetMain();
        Vector3 WorldToScreenPoint(const Vector3& pos);
    };

    struct Screen
    {
        static int GetHeight();
        static int GetWidth();
    };

    struct Vector3
    {


        float x, y, z;
    };

    struct Vector3_Boxed : unsafe::Object
    {
        Vector3_Boxed(float x, float y, float z);
        Vector3_Boxed(const Vector3& v);

        Vector3 m_Value;
    };

    struct Quaternion
    {
        Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

        static Unity::Quaternion identity() { return Quaternion(0.f, 0.f, 0.f, 1.f); }

        float x;
        float y;
        float z;
        float w;
    };

    struct Quaternion_Boxed : unsafe::Object
    {
        Quaternion_Boxed(float x, float y, float z, float w);
        Quaternion_Boxed(const Quaternion& v);

        Quaternion m_Value;
    };

    struct GameObject : Object
    {
        explicit GameObject(unsafe::Object* ptr) : Object(ptr) {}
    };
}

template <>
inline System::Type typeof<Unity::Object> = System::Type("UnityEngine.CoreModule", "UnityEngine", "Object");

template <>
inline System::Type typeof<Unity::Camera> = System::Type("UnityEngine.CoreModule", "UnityEngine", "Camera");

template <>
inline System::Type typeof<Unity::Component> = System::Type("UnityEngine.CoreModule", "UnityEngine", "Component");

template <>
inline System::Type typeof<Unity::Transform> = System::Type("UnityEngine.CoreModule", "UnityEngine", "Transform");

template <>
inline System::Type typeof<Unity::Screen> = System::Type("UnityEngine.CoreModule", "UnityEngine", "Screen");

template <>
inline System::Type typeof<Unity::Vector3> = System::Type("UnityEngine.CoreModule", "UnityEngine", "Vector3");

template <>
inline System::Type typeof<Unity::GameObject> = System::Type("UnityEngine.CoreModule", "UnityEngine", "GameObject");

