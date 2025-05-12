#include "haxsdk_unity.h"

namespace Unity
{
    Vector3 Transform::GetPosition()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Transform>.GetMethod("get_position"));
        if (HaxSdk::IsMono())
        {
            return method.m_Thunk 
                ? method.Thunk<Vector3_Boxed*, Transform>(*this)->m_Value 
                : method.Invoke<Vector3_Boxed*, Transform>(*this)->m_Value;
        }
        return method.Address<Vector3, Transform>(*this);
    }

    void Transform::SetPosition(const Unity::Vector3& value)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Transform>.GetMethod("set_position"));
        if (HaxSdk::IsMono())
        {
            method.m_Thunk
                ? method.Thunk<void, Transform, Vector3_Boxed>(*this, Vector3_Boxed(value))
                : method.Invoke<void, Transform, Vector3_Boxed>(*this, Vector3_Boxed(value));
        }
        return method.Address<void, Transform, Vector3>(*this, value);
    }

    Transform Transform::GetParent()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Transform>.GetMethod("get_parent"));
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
        CHECK_NULL();

        static System::MethodInfo method(typeof<Transform>.GetMethod("set_parent"));
        if (HaxSdk::IsMono())
        {
            method.m_Thunk
                ? method.Thunk<Transform, Transform>(*this)
                : method.Invoke<Transform, Transform>(*this);
        }
        method.Address<Transform, Transform>(*this);
    }

    Vector3 Transform::GetForward()
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Transform>.GetMethod("get_forward"));
        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<Vector3_Boxed*, Transform>(*this)->m_Value
                : method.Invoke<Vector3_Boxed*, Transform>(*this)->m_Value;
        }
        return method.Address<Vector3, Transform>(*this);
    }
}