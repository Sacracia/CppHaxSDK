#include "haxsdk_unity.h"

#include "haxsdk.h"

template <>
System::Type typeof<Unity::Vector3>()
{
    return System::Assembly::GetUnityCoreLib().GetType("UnityEngine", "Vector3", true);
}

namespace Unity
{
    System::Type Object::TypeOf()
    {
        static auto type = System::Assembly::GetUnityCoreLib().GetType("UnityEngine", "Object", true);
        return type;
    }

    Object Object::Instantiate(const Object& original)
    {
        static HaxMethod method = TypeOf().GetMethod("Instantiate", "UnityEngine.Object(UnityEngine.Object)", true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Object, Object>(original)
                : method.InvokeStatic<Object, Object>(original);
        }

        return method.Address<Object, Object>(original);
    }

    Object Object::Instantiate(const Object& original, const Vector3& position, const Quaternion& rotation)
    {
        static HaxMethod method = TypeOf().GetMethod("Instantiate", "UnityEngine.Object(UnityEngine.Object,UnityEngine.Vector3,UnityEngine.Quaternion)", true);

        if (HaxSdk::IsMono())
            return method.m_Thunk
            ? method.Thunk<Object, Object, Vector3, Quaternion>(original, position, rotation)
            : method.InvokeStatic<Object, Object, Vector3, Quaternion>(original, position, rotation);

        return method.Address<Object, Object, Vector3, Quaternion>(original, position, rotation);
    }

    System::Array<Object> Object::FindObjectsOfType(const System::Type& type)
    {
        static HaxMethod method = TypeOf().GetMethod("FindObjectsOfType", "UnityEngine.Object[](System.Type)", true);

        if (HaxSdk::IsMono())
            return method.m_Thunk
            ? method.Thunk<System::Array<Object>, System::Type>(type)
            : method.InvokeStatic<System::Array<Object>, System::Type>(type);

        return method.Address<System::Array<Object>, System::Type>(type);
    }

    Object Object::FindObjectOfType(const System::Type& type)
    {
        static HaxMethod method = TypeOf().GetMethod("FindObjectOfType", "UnityEngine.Object(System.Type)", true);

        if (HaxSdk::IsMono())
            return method.m_Thunk
            ? method.Thunk<Object, System::Type>(type)
            : method.InvokeStatic<Object, System::Type>(type);

        return method.Address<Object, System::Type>(type);
    }

    void Object::Destroy(const Object& obj, float t)
    {
        static HaxMethod method = TypeOf().GetMethod("Destroy", nullptr, true);

        if (HaxSdk::IsMono())
        {
            method.m_Thunk
                ? method.Thunk<void, Object, float>(obj, t)
                : method.Invoke<void, Object, float>(obj, t);
            return;
        }

        method.Address<void, Object, float>(obj, t);
    }

    System::String Object::GetName() const
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_name", nullptr, true);

        if (HaxSdk::IsMono())
            return method.m_Thunk
            ? method.Thunk<System::String, Object>(*this)
            : method.Invoke<System::String, Object>(*this);

        return method.Address<System::String, Object>(*this);
    }

    void Object::SetName(const System::String& name) const
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("set_name", nullptr, true);

        if (HaxSdk::IsMono())
        {
            method.m_Thunk
                ? method.Thunk<void, Object, System::String>(*this, name)
                : method.Invoke<void, Object, System::String>(*this, name);
            return;
        }

        method.Address<void, Object, System::String>(*this, name);
    }

    void Object::SetHideFlags(HideFlags flags) const
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("set_hideFlags", nullptr, true);

        if (HaxSdk::IsMono())
        {
            method.m_Thunk
                ? method.Thunk<void, Object, HideFlags>(*this, flags)
                : method.Invoke<void, Object, HideFlags>(*this, flags);
            return;
        }

        method.Address<void, Object, HideFlags>(*this, flags);
    }

    void* Object::GetCachedPtr() const
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("GetCachedPtr");
        if (HaxSdk::IsMono())
            return method.m_Thunk ? method.Thunk<void*, Object>(*this) : method.Invoke<void*, Object>(*this);

        return method.Address<void*, Object>(*this);
    }

    // GameObject
    System::Type GameObject::TypeOf()
    {
        static System::Type type = System::Assembly::GetUnityCoreLib().GetType("UnityEngine", "GameObject", true);
        return type;
    }

    GameObject GameObject::New(const char* name)
    {
        static HaxMethod method = TypeOf().GetMethod(".ctor", "System.Void(System.String)", true);

        GameObject go = HaxSdk::IsMono() ? GameObject(Mono::AllocObject(TypeOf().m_MonoType)) : GameObject(Il2Cpp::AllocObject(TypeOf().m_Il2CppType));
        System::String str = System::String::New(name);

        if (HaxSdk::IsMono())
        {
            method.m_Thunk
                ? method.Thunk<void, GameObject, System::String>(go, str)
                : method.Invoke<void, GameObject, System::String>(go, str);
            return go;
        }

        method.Address<void, GameObject, System::String>(go, str);
        return go;
    }

    Transform GameObject::GetTransform()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_transform");
        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Transform, GameObject>(*this)
                : method.Invoke<Transform, GameObject>(*this);
        }

        return method.Address<Transform, GameObject>(*this);
    }

    void GameObject::SetLayer(Int32 value)
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("set_layer");
        if (HaxSdk::IsMono())
        {
            method.m_Thunk
                ? method.Thunk<void, GameObject, Int32>(*this, value)
                : method.Invoke<void, GameObject, Int32>(*this, value);
            return;
        }

        method.Address<void, GameObject, Int32>(*this, value);
    }

    bool GameObject::GetActiveSelf()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_activeSelf", nullptr, true);
        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<bool, GameObject>(*this)
                : method.Invoke<bool, GameObject>(*this);
        }

        return method.Address<bool, GameObject>(*this);
    }

    Component GameObject::GetComponent(const System::Type& type)
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("GetComponent", "UnityEngine.Component(System.Type)", true);
        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Component, GameObject, System::Type>(*this, type)
                : method.Invoke<Component, GameObject, System::Type>(*this, type);
        }

        return method.Address<Component, GameObject, System::Type>(*this, type);
    }

    Component GameObject::AddComponent(const System::Type& componentType)
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("AddComponent", "UnityEngine.Component(System.Type)", true);
        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Component, GameObject, System::Type>(*this, componentType)
                : method.Invoke<Component, GameObject, System::Type>(*this, componentType);
        }

        return method.Address<Component, GameObject, System::Type>(*this, componentType);
    }

    System::Array<Component> GameObject::GetComponentsInChildren(const System::Type& type, bool includeInactive)
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("GetComponentsInChildren", "Component[](System.Type,System.Boolean)", true);
        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<System::Array<Component>, GameObject, System::Type, bool>(*this, type, includeInactive)
                : method.Invoke<System::Array<Component>, GameObject, System::Type, bool>(*this, type, includeInactive);
        }

        return method.Address<System::Array<Component>, GameObject, System::Type, bool>(*this, type, includeInactive);
    }

    void GameObject::SetActive(bool value)
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("set_active", nullptr, true);
        if (HaxSdk::IsMono())
        {
            method.m_Thunk
                ? method.Thunk<void, GameObject, bool>(*this, value)
                : method.Invoke<void, GameObject, bool>(*this, value);
            return;
        }

        method.Address<void, GameObject, bool>(*this, value);
    }

    bool GameObject::GetActive()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_active", nullptr, true);
        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<bool, GameObject>(*this)
                : method.Invoke<bool, GameObject>(*this);
        }

        return method.Address<bool, GameObject>(*this);
    }

    bool GameObject::GetActiveInHierarchy()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_activeInHierarchy", nullptr, true);
        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<bool, GameObject>(*this)
                : method.Invoke<bool, GameObject>(*this);
        }

        return method.Address<bool, GameObject>(*this);
    }

    // Component
    System::Type Component::TypeOf()
    {
        static System::Type type = System::Assembly::GetUnityCoreLib().GetType("UnityEngine", "Component", true);
        return type;
    }

    Transform Component::GetTransform()
    {
        if (IsNull())
        {
            printf("NULL\n");
            throw System::NullReferenceException::New();
        }

        static HaxMethod method = TypeOf().GetMethod("get_transform", nullptr, true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Transform, Component>(*this)
                : method.Invoke<Transform, Component>(*this);
        }

        return method.Address<Transform, Component>(*this);
    }

    GameObject Component::GetGameObject()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_gameObject", nullptr, true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<GameObject, Component>(*this)
                : method.Invoke<GameObject, Component>(*this);
        }

        return method.Address<GameObject, Component>(*this);
    }

    Component Component::GetComponentInChildren(System::Type type)
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("GetComponentInChildren", "UnityEngine.Component(System.Type)", true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Component, Component, System::Type>(*this, type)
                : method.Invoke<Component, Component, System::Type>(*this, type);
        }

        return method.Address<Component, Component, System::Type>(*this, type);
    }

    System::Array<Component> Component::GetComponentsInChildren(System::Type type)
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("GetComponentsInChildren", "UnityEngine.Component[](System.Type)", true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<System::Array<Component>, Component, System::Type>(*this, type)
                : method.Invoke<System::Array<Component>, Component, System::Type>(*this, type);
        }

        return method.Address<System::Array<Component>, Component, System::Type>(*this, type);
    }

    Component Component::GetComponent(System::Type type)
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("GetComponent", "UnityEngine.Component(System.Type)", true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Component, Component, System::Type>(*this, type)
                : method.Invoke<Component, Component, System::Type>(*this, type);
        }

        return method.Address<Component, Component, System::Type>(*this, type);
    }

    // Transform
    System::Type Transform::TypeOf()
    {
        static System::Type type = System::Assembly::GetUnityCoreLib().GetType("UnityEngine", "Transform", true);
        return type;
    }

    Vector3 Transform::GetPosition()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_position", nullptr, true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Vector3, Transform>(*this)
                : method.Invoke<Vector3, Transform>(*this);
        }

        return method.Address<Vector3, Transform>(*this);
    }

    void Transform::SetPosition(const Unity::Vector3& value)
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("set_position", nullptr, true);

        if (HaxSdk::IsMono())
        {
            method.m_Thunk
                ? method.Thunk<void, Transform, Vector3>(*this, value)
                : method.Invoke<void, Transform, Vector3>(*this, value);
            return;
        }

        method.Address<void, Transform, Vector3>(*this, value);
    }

    Transform Transform::GetParent()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_position", nullptr, true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Transform, Transform>(*this)
                : method.Invoke<Transform, Transform>(*this);
        }

        return method.Address<Transform, Transform>(*this);
    }

    void Transform::SetParent(const Unity::Transform& value)
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("set_parent", nullptr, true);

        if (HaxSdk::IsMono())
        {
            method.m_Thunk
                ? method.Thunk<void, Transform, Transform>(*this, value)
                : method.Invoke<void, Transform, Transform>(*this, value);
            return;
        }

        method.Address<void, Transform, Transform>(*this, value);
    }

    Vector3 Transform::GetForward()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_forward", nullptr, true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Vector3, Transform>(*this)
                : method.Invoke<Vector3, Transform>(*this);
        }

        return method.Address<Vector3, Transform>(*this);
    }

    // Behaviour
    System::Type Behaviour::TypeOf()
    {
        static System::Type type = System::Assembly::GetUnityCoreLib().GetType("UnityEngine", "Behaviour", true);
        return type;
    }

    bool Behaviour::GetEnabled()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_enabled", nullptr, true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<bool, Behaviour>(*this)
                : method.Invoke<bool, Behaviour>(*this);
        }

        return method.Address<bool, Behaviour>(*this);
    }

    bool Behaviour::GetIsActiveAndEnabled()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_isActiveAndEnabled", nullptr, true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<bool, Behaviour>(*this)
                : method.Invoke<bool, Behaviour>(*this);
        }

        return method.Address<bool, Behaviour>(*this);
    }

    void Behaviour::SetEnabled(bool value)
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("set_enabled", nullptr, true);

        if (HaxSdk::IsMono())
        {
            method.m_Thunk
                ? method.Thunk<void, Behaviour, bool>(*this, value)
                : method.Invoke<void, Behaviour, bool>(*this, value);
            return;
        }

        method.Address<void, Behaviour, bool>(*this, value);
    }

    // Camera
    System::Type Camera::TypeOf()
    {
        static System::Type type = System::Assembly::GetUnityCoreLib().GetType("UnityEngine", "Camera", true);
        return type;
    }

    Camera Camera::GetMain()
    {
        static HaxMethod method = TypeOf().GetMethod("get_main", nullptr, true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Camera>()
                : method.InvokeStatic<Camera>();
        }

        return method.Address<Camera>();
    }

    Vector3 Camera::WorldToScreenPoint(const Vector3& position)
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("WorldToScreenPoint", "UnityEngine.Vector3(UnityEngine.Vector3)", true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Vector3_B, Camera, Vector3_B>(*this, ::Box(position)).Unbox()
                : method.Invoke<Vector3, Camera, Vector3>(*this, position);
        }

        return method.Address<Vector3, Camera, Vector3>(*this, position);
    }

    float Camera::GetOrthographicSize()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_orthographicSize", nullptr, true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<float, Camera>(*this)
                : method.Invoke<float, Camera>(*this);
        }

        return method.Address<float, Camera>(*this);
    }

    void Camera::SetOrthographicSize(float value)
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("set_orthographicSize", nullptr, true);

        if (HaxSdk::IsMono())
        {
            method.m_Thunk
                ? method.Thunk<void, Camera, float>(*this, value)
                : method.Invoke<void, Camera, float>(*this, value);
            return;
        }

        method.Address<void, Camera, float>(*this, value);
    }

    Int32 Camera::GetPixelWidth()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_pixelWidth", nullptr, true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Int32, Camera>(*this)
                : method.Invoke<Int32, Camera>(*this);
        }

        return method.Address<Int32, Camera>(*this);
    }

    Int32 Camera::GetPixelHeight()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_pixelHeight", nullptr, true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Int32, Camera>(*this)
                : method.Invoke<Int32, Camera>(*this);
        }

        return method.Address<Int32, Camera>(*this);
    }

    Matrix4x4 Camera::GetProjectionMatrix()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_projectionMatrix", nullptr, true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Matrix4x4, Camera>(*this)
                : method.Invoke<Matrix4x4, Camera>(*this);
        }

        return method.Address<Matrix4x4, Camera>(*this);
    }

    Matrix4x4 Camera::GetWorldToCameraMatrix()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_worldToCameraMatrix", nullptr, true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Matrix4x4, Camera>(*this)
                : method.Invoke<Matrix4x4, Camera>(*this);
        }

        return method.Address<Matrix4x4, Camera>(*this);
    }

    float Camera::GetFarClipPlane()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_farClipPlane", nullptr, true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<float, Camera>(*this)
                : method.Invoke<float, Camera>(*this);
        }

        return method.Address<float, Camera>(*this);
    }

    void Camera::SetFarClipPlane(float value)
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("set_farClipPlane", nullptr, true);

        if (HaxSdk::IsMono())
        {
            method.m_Thunk
                ? method.Thunk<void, Camera, float>(*this, value)
                : method.Invoke<void, Camera, float>(*this, value);
            return;
        }

        method.Address<void, Camera, float>(*this, value);
    }

    float Camera::GetNearClipPlane()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_nearClipPlane", nullptr, true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<float, Camera>(*this)
                : method.Invoke<float, Camera>(*this);
        }

        return method.Address<float, Camera>(*this);
    }

    float Camera::GetFieldOfView()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        static HaxMethod method = TypeOf().GetMethod("get_fieldOfView", nullptr, true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<float, Camera>(*this)
                : method.Invoke<float, Camera>(*this);
        }

        return method.Address<float, Camera>(*this);
    }

    // Screen
    System::Type Screen::TypeOf()
    {
        static System::Type type = System::Assembly::GetUnityCoreLib().GetType("UnityEngine", "Screen", true);
        return type;
    }

    Int32 Screen::GetWidth()
    {
        static HaxMethod method = TypeOf().GetMethod("get_width", nullptr, true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Int32>()
                : method.InvokeStatic<Int32>();
        }

        return method.Address<Int32>();
    }

    Int32 Screen::GetHeight()
    {
        static HaxMethod method = TypeOf().GetMethod("get_height", nullptr, true);

        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Int32>()
                : method.InvokeStatic<Int32>();
        }

        return method.Address<Int32>();
    }

    System::Type Vector3::TypeOf()
    {
        static System::Type type = System::Assembly::GetUnityCoreLib().GetType("UnityEngine", "Vector3", true);
        return type;
    }
}