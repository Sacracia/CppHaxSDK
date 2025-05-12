#include "haxsdk_unity.h"

#undef CHECK_NULL
#define CHECK_NULL() if (Null()) throw System::NullReferenceException::New(); 

namespace vtables
{
    static unsafe::VTable* g_Vector3;
    static unsafe::VTable* g_Quaternion;
}

static bool g_Initialized = false;

void HaxSdk::InitUnity()
{
    if (!g_Initialized)
    {
        g_Initialized = true;

        HaxSdk::InitSystem();

        vtables::g_Vector3 = unsafe::Image::GetUnityCore()->GetClass("UnityEngine", "Vector3")->GetVTable();
    }
}

namespace Unity
{
    Camera Camera::GetMain()
    {
        static System::MethodInfo method(typeof<Camera>.GetMethod("get_main"));
        if (HaxSdk::IsMono())
            return method.m_Thunk ? method.Thunk<Camera>() : method.InvokeStatic<Camera>();
        return method.Address<Camera>();
    }

    Vector3 Camera::WorldToScreenPoint(const Vector3& pos)
    {
        CHECK_NULL();

        static System::MethodInfo method(typeof<Camera>.GetMethod("WorldToScreenPoint", "UnityEngine.Vector3(UnityEngine.Vector3)"));
        if (HaxSdk::IsMono())
        {
            Vector3_Boxed boxed(pos);
            return method.m_Thunk
                ? method.Thunk<Vector3_Boxed*, Camera, Vector3_Boxed*>(*this, &boxed)->m_Value
                : method.Invoke<Vector3_Boxed*, Camera, Vector3>(*this, pos)->m_Value;
        }
        return method.Address<Vector3, Camera, Vector3>(*this, pos);
    }

    int Screen::GetHeight()
    {
        static System::MethodInfo method(typeof<Screen>.GetMethod("get_height"));
        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<int>()
                : method.InvokeStatic<System::Int32_Boxed*>()->m_Value;
        }
        return method.Address<int>();
    }

    int Screen::GetWidth()
    {
        static System::MethodInfo method(typeof<Screen>.GetMethod("get_width"));
        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<int>()
                : method.InvokeStatic<System::Int32_Boxed*>()->m_Value;
        }
        return method.Address<int>();
    }

    Vector3_Boxed::Vector3_Boxed(float _x, float _y, float _z) : Object(vtables::g_Vector3), m_Value(Vector3(_x, _y, _z))
    {

    }

    Vector3_Boxed::Vector3_Boxed(const Vector3& v) : Object(vtables::g_Vector3), m_Value(v)
    {

    }

    Quaternion_Boxed::Quaternion_Boxed(float x, float y, float z, float w) : Object(vtables::g_Quaternion), m_Value(x, y, z, w)
    {

    }

    Quaternion_Boxed::Quaternion_Boxed(const Quaternion& v) : Object(vtables::g_Quaternion), m_Value(v)
    {

    }
}