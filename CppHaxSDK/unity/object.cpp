#include "haxsdk_unity.h"

namespace Unity
{
    Object Object::Instantiate(const Object& original)
    {
        static System::MethodInfo method = typeof<Object>.GetMethod("Instantiate", "UnityEngine.Object(UnityEngine.Object)");
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
        static System::MethodInfo method = typeof<Object>.GetMethod("Instantiate", "UnityEngine.Object(UnityEngine.Object,UnityEngine.Vector3,UnityEngine.Quaternion)");
        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Object, Object, Vector3_Boxed, Quaternion_Boxed>(original, Vector3_Boxed(position), Quaternion_Boxed(rotation))
                : method.InvokeStatic<Object, Object, Vector3, Quaternion>(original, position, rotation);
        }
        return method.Address<Object, Object, Vector3, Quaternion>(original, position, rotation);
    }

    System::Array<Object> Object::FindObjectsOfType(System::Type type)
    {
        static System::MethodInfo method = typeof<Object>.GetMethod("FindObjectsOfType", "UnityEngine.Object[](System.Type)");
        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<System::Array<Object>, System::Type>(type)
                : method.InvokeStatic<System::Array<Object>, System::Type>(type);
        }
        return method.Address<System::Array<Object>, System::Type>(type);
    }

    Object Object::FindObjectOfType(System::Type type)
    {
        static System::MethodInfo method = typeof<Object>.GetMethod("FindObjectOfType", "UnityEngine.Object(System.Type)");
        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Object, System::Type>(type)
                : method.InvokeStatic<Object, System::Type>(type);
        }
        return method.Address<Object, System::Type>(type);
    }

    void Object::Destroy(Object obj, float t)
    {
        static System::MethodInfo method = typeof<Object>.GetMethod("Destroy");
        if (HaxSdk::IsMono())
        {
            method.m_Thunk
                ? method.Thunk<void, Object, float>(obj, t)
                : method.InvokeStatic<void, Object, float>(obj, t);
        }
        method.Address<void, Object, float>(obj, t);
    }

    System::String Object::GetName()
    {
        CHECK_NULL();

        static System::MethodInfo method = typeof<Object>.GetMethod("get_name");
        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<System::String, Object>(*this)
                : method.Invoke<System::String, Object>(*this);
        }
        return method.Address<System::String, Object>(*this);
    }

    void Object::SetName(System::String name)
    {
        CHECK_NULL();

        static System::MethodInfo method = typeof<Object>.GetMethod("set_name");
        if (HaxSdk::IsMono())
        {
            method.m_Thunk
                ? method.Thunk<void, Object, System::String>(*this, name)
                : method.Invoke<void, Object, System::String>(*this, name);
        }
        method.Address<void, Object, System::String>(*this, name);
    }

    void Object::SetHideFlags(HideFlags flags)
    {
        CHECK_NULL();

        static System::MethodInfo method = typeof<Object>.GetMethod("set_hideFlags");
        if (HaxSdk::IsMono())
        {
            method.m_Thunk
                ? method.Thunk<void, Object, HideFlags>(*this, flags)
                : method.Invoke<void, Object, HideFlags>(*this, flags);
        }
        method.Address<void, Object, HideFlags>(*this, flags);
    }
}