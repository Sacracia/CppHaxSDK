#pragma once

#include "haxsdk_system.h"

namespace Unity
{
    struct AsyncOperation;
    struct Behaviour;
    struct BoxCollider;
    struct Bounds;
    struct Camera;
    struct Component;
    struct Collider;
    struct Collider2D;
    struct Color;
    struct GameObject;
    struct KeyCode;
    struct Light;
    struct LineRenderer;
    struct Object;
    struct Renderer;
    struct Rect;
    struct Screen;
    struct Sprite;
    struct Shader;
    struct Material;
    struct Matrix4x4;
    struct Transform;
    struct Vector2;
    struct Vector3;
    struct Quaternion;
}

namespace Unity
{
    enum HideFlags : int32_t {
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
        explicit                                Object(const System::Object& o) : System::Object(o) {}

        static System::Type                     TypeOf();

        static Object                           Instantiate(const Object& original);
        static Object                           Instantiate(const Object& original, const Vector3& position, const Quaternion& rotation);
        static System::Array<Object>            FindObjectsOfType(const System::Type& type);
        static Object                           FindObjectOfType(const System::Type& type);
        static void                             Destroy(const Object& obj, float t = 0.f);

        System::String                          GetName() const;
        void                                    SetName(const System::String& name) const;
        void                                    SetHideFlags(HideFlags flags) const;
        void*                                   GetCachedPtr() const;

        inline bool                             IsNull() const { return !m_UnityObject || !m_UnityObject->m_CachedPtr; }
    };

    struct GameObject : Object
    {
        explicit                                GameObject(const System::Object& o) : Object(o) {}

        static System::Type                     TypeOf();
        static inline GameObject                New() { return GameObject(TypeOf().CreateInstance()); }
        static GameObject                       New(const char* name);

        Transform                               GetTransform();
        void                                    SetLayer(Int32 value);
        bool                                    GetActiveSelf();
        Component                               GetComponent(const System::Type& type);
        Component                               AddComponent(const System::Type& componentType);
        System::Array<Component>                GetComponentsInChildren(const System::Type& pType, bool includeInactive);
        void                                    SetActive(bool value);
        bool                                    GetActive();
        bool                                    GetActiveInHierarchy();
    };

    struct Component : Object 
    {
        explicit                                Component(const System::Object& o) : Object(o) {}

        static System::Type                     TypeOf();

        Transform                               GetTransform();
        GameObject                              GetGameObject();
        Component                               GetComponentInChildren(System::Type type);
        System::Array<Component>                GetComponentsInChildren(System::Type type);
        Component                               GetComponent(System::Type type);
    };

    struct Transform : Component 
    {
        explicit                                Transform(const System::Object& o) : Component(o) {}

        static System::Type                     TypeOf();

        Vector3                                 GetPosition();
        void                                    SetPosition(const Unity::Vector3& value);
        Transform                               GetParent();
        void                                    SetParent(const Unity::Transform& value);
        Vector3                                 GetForward();
    };

    struct Behaviour : Component {
                                                Behaviour(const System::Object& o) : Component(o) {}

        static System::Type                     TypeOf();

        bool                                    GetEnabled();
        bool                                    GetIsActiveAndEnabled();
        void                                    SetEnabled(bool value);
    };

    struct Camera : Behaviour {
        enum MonoOrStereoscopicEye : Int32 {
            Left,
            Right,
            Mono
        };

                                                Camera(const System::Object& o) : Behaviour(o) {}

        static System::Type                     TypeOf();
        static Camera                           GetMain();

        Vector3                                 WorldToScreenPoint(const Vector3& position);

        float                                   GetOrthographicSize();
        void                                    SetOrthographicSize(float value);

        Int32                                   GetPixelWidth();
        Int32                                   GetPixelHeight();
        Matrix4x4                               GetProjectionMatrix();
        Matrix4x4                               GetWorldToCameraMatrix();
        float                                   GetFarClipPlane();
        void                                    SetFarClipPlane(float value);
        float                                   GetNearClipPlane();
        float                                   GetFieldOfView();
    };

    struct Screen : System::Object
    {
        static System::Type                     TypeOf();
        static Int32                            GetWidth();
        static Int32                            GetHeight();
    };

    struct Matrix4x4 {
        //Vector4 operator*(const Vector4& v);

        //Vector3 MultiplyPoint(Vector3& point);

        float m00, m10, m20, m30;
        float m01, m11, m21, m31;
        float m02, m12, m22, m32;
        float m03, m13, m23, m33;
    };

    struct Vector3 : System::ValueType
    {
        static System::Type TypeOf();

        float x, y, z;
    };

    struct Vector3_B : System::Object
    {
        Vector3_B(const System::Object& o) : System::Object(o) {}

        inline Vector3 Unbox() { if (!m_ManagedPtr) throw System::NullReferenceException::New(); return *(Vector3*)((char*)m_ManagedPtr + sizeof(MonoObject)); }
    };

    struct Quaternion
    {
        float x;
        float y;
        float z;
        float w;
    };
}

template <>
extern System::Type typeof<Unity::Vector3>();

inline Unity::Vector3_B Box(const Unity::Vector3& v)
{
    return HaxSdk::IsMono() 
        ? Unity::Vector3_B(Mono::BoxValue(typeof<Unity::Vector3>().m_MonoType, (void*)&v)) 
        : Unity::Vector3_B(Il2Cpp::BoxValue(typeof<Unity::Vector3>().m_Il2CppType, (void*)&v));
}