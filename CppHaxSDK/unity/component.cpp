#include "haxsdk_unity.h"

namespace Unity
{
    Transform Component::GetTransform()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Component>.GetMethod("get_transform"));
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
        CHECK_NULL();

        static System::MethodInfo method = typeof<Component>.GetMethod("get_gameObject");
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
        CHECK_NULL();

        static System::MethodInfo method = typeof<Component>.GetMethod("GetComponentInChildren", "UnityEngine.Component(System.Type)");
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
        CHECK_NULL();

        static System::MethodInfo method = typeof<Component>.GetMethod("GetComponentsInChildren", "UnityEngine.Component[](System.Type)");
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
        CHECK_NULL();

        static System::MethodInfo method = typeof<Component>.GetMethod("GetComponent", "UnityEngine.Component(System.Type)");
        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Component, Component, System::Type>(*this, type)
                : method.Invoke<Component, Component, System::Type>(*this, type);
        }
        return method.Address<Component, Component, System::Type>(*this, type);
    }
}