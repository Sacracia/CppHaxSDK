#include "haxsdk_backend.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include <vector>

#include "../haxsdk_assertion.h"
#include "../haxsdk_tools.h"
#include "mono/haxsdk_mono.h"
#include "il2cpp/haxsdk_il2cpp.h"

static HaxBackend GetBackend(OUT HMODULE& handle);

static HaxBackend g_Backend = HaxBackend_None;
static HMODULE g_BackendHandle;
static bool g_Initialized = false;

namespace Metadata
{
    static void* Int32;
    static void* Vector3;
}

static void* GetMetadata(backend::Image* image, const char* nameSpace, const char* name)
{
    if (HaxSdk::IsMono())
    {
        MonoClass* klass = mono::Class::FromName((MonoImage*)image, nameSpace, name);
        return mono::Class::VTable(mono::Domain::GetRoot(), klass);
    }
    return il2cpp::Class::FromName((Il2CppImage*)image, nameSpace, name);
}

namespace HaxSdk
{
    void InitBackend()
    {
        if (!g_Initialized)
        {
            g_Backend = GetBackend(g_BackendHandle);
            HAX_ASSERT(g_Backend);
            IsMono() ? mono::Initialize() : il2cpp::Initialize();

            Metadata::Int32 = GetMetadata(backend::Image::GetCorlib(), "System", "Int32");
            Metadata::Vector3 = GetMetadata(backend::Assembly::GetImage(backend::Assembly::Load("UnityEngine.CoreModule")), "UnityEngine", "Vector3");

            g_Initialized = true;
        }
    }

    void AttachThread()
    {
        if (IsMono())
        {
            mono::Thread::Attach(mono::Domain::GetRoot());
            return;
        }
        il2cpp::Thread::Attach(il2cpp::Domain::GetCurrent());
    }

    bool IsMono()
    {
        return g_Backend == HaxBackend_Mono;
    }

    bool IsIl2Cpp()
    {
        return g_Backend == HaxBackend_Il2Cpp;
    }

    void* GetBackendHandle()
    {
        return g_BackendHandle;
    }
}

static HaxBackend GetBackend(OUT HMODULE& handle)
{
    MODULEENTRY32W me = { 0 };
    me.dwSize = sizeof(MODULEENTRY32W);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
    if (Module32FirstW(snapshot, &me))
    {
        do
        {
            if (wcsstr(me.szModule, L"mono") != nullptr)
            {
                handle = me.hModule;
                CloseHandle(snapshot);
                return HaxBackend_Mono;
            }
            if (wcscmp(me.szModule, L"GameAssembly.dll") == 0)
            {
                handle = me.hModule;
                CloseHandle(snapshot);
                return HaxBackend_Il2Cpp;
            }
        } while (Module32NextW(snapshot, &me));
    }

    CloseHandle(snapshot);
    return HaxBackend_None;
}

namespace backend
{
    Vector3_Boxed::Vector3_Boxed(float _x, float _y, float _z) : x(_x), y(_y), z(_z), metadata(Metadata::Vector3), monitor(nullptr) {}

    Image* Assembly::GetImage(Assembly* assembly)
    {
        return HaxSdk::IsMono()
            ? (Image*)(mono::Assembly::GetImage((MonoAssembly*)assembly))
            : (Image*)(il2cpp::Assembly::GetImage((Il2CppAssembly*)assembly));
    }

    Assembly* Assembly::Load(const char* name)
    {
        if (HaxSdk::IsMono())
        {
            MonoDomain* domain = mono::Domain::GetRoot();
            return (Assembly*)mono::Assembly::Load(domain, name);
        }

        Il2CppDomain* domain = il2cpp::Domain::GetCurrent();
        return (Assembly*)il2cpp::Assembly::Load(domain, name);
    }

    Class* Class::FromName(Image* image, const char* nameSpace, const char* name)
    {
        return HaxSdk::IsMono()
            ? (Class*)mono::Class::FromName((MonoImage*)image, nameSpace, name)
            : (Class*)il2cpp::Class::FromName((Il2CppImage*)image, nameSpace, name);
    }

    Field* Class::GetFieldFromName(Class* klass, const char* name)
    {
        return HaxSdk::IsMono()
            ? (Field*)mono::Class::GetFieldFromName((MonoClass*)klass, name)
            : (Field*)il2cpp::Class::GetFieldFromName((Il2CppClass*)klass, name);
    }

    Method* Class::GetMethodFromName(Class* klass, const char* name, const char* signature)
    {
        return HaxSdk::IsMono()
            ? (Method*)mono::Class::GetMethodFromName((MonoClass*)klass, name, signature)
            : (Method*)il2cpp::Class::GetMethodFromName((Il2CppClass*)klass, name, signature);
    }

    Type* Class::GetType(Class* klass)
    {
        return HaxSdk::IsMono()
            ? (Type*)mono::Class::GetType((MonoClass*)klass)
            : (Type*)il2cpp::Class::GetType((Il2CppClass*)klass);
    }

    Domain* Domain::GetCurrent()
    {
        return HaxSdk::IsMono()
            ? (Domain*)mono::Domain::GetCurrent()
            : (Domain*)il2cpp::Domain::GetCurrent();
    }

    Image* Image::GetCorlib()
    {
        return HaxSdk::IsMono()
            ? (Image*)mono::Image::GetCorlib()
            : (Image*)il2cpp::Image::GetCorlib();
    }

    Domain* Domain::GetRoot()
    {
        return HaxSdk::IsMono()
            ? (Domain*)mono::Domain::GetRoot()
            : (Domain*)il2cpp::Domain::GetCurrent();
    }

    const char* Exception::GetMessage(Exception* ex)
    {
        String* str = HaxSdk::IsMono()
            ? (String*)mono::Exception::GetMessage((MonoException*)ex)
            : (String*)il2cpp::Exception::GetMessage((Il2CppException*)ex);
        return HaxSdk::UTF8(str->m_Chars, str->m_Length);
    }

    Exception* Exception::GetNullReferenceException()
    {
        return HaxSdk::IsMono()
            ? (Exception*)mono::Exception::GetNullReferenceException()
            : (Exception*)il2cpp::Exception::GetNullReferenceException();
    }

    Exception* Exception::GetArgumentOutOfRangeException()
    {
        return HaxSdk::IsMono()
            ? (Exception*)mono::Exception::GetArgumentOutOfRangeException()
            : (Exception*)il2cpp::Exception::GetArgumentOutOfRangeException();
    }

    void* Field::GetAddress(Field* field, Object* __this)
    {
        return HaxSdk::IsMono()
            ? mono::Field::GetAddress((MonoClassField*)field, (MonoObject*)__this)
            : il2cpp::Field::GetAddress((Il2CppField*)field, (Il2CppObject*)__this);
    }

    Class* Field::GetParent(Field* field)
    {
        return HaxSdk::IsMono()
            ? (Class*)mono::Field::GetParent((MonoClassField*)field)
            : (Class*)il2cpp::Field::GetParent((Il2CppField*)field);
    }

    void Field::StaticGetValue(Field* field, void* value)
    {
        if (HaxSdk::IsMono())
        {
            MonoClass* klass = mono::Field::GetParent((MonoClassField*)field);
            MonoVTable* vtable = mono::Class::VTable(mono::Domain::GetRoot(), klass);
            mono::Field::StaticGetValue(vtable, (MonoClassField*)field, value);
            return;
        }
        il2cpp::Field::StaticGetValue((Il2CppField*)field, value);
    }

    void Field::StaticSetValue(Field* field, void* value)
    {
        if (HaxSdk::IsMono())
        {
            MonoClass* klass = mono::Field::GetParent((MonoClassField*)field);
            MonoVTable* vtable = mono::Class::VTable(mono::Domain::GetRoot(), klass);
            mono::Field::StaticSetValue(vtable, (MonoClassField*)field, value);
            return;
        }
        il2cpp::Field::StaticSetValue((Il2CppField*)field, value);
    }

    ReflectionType* Reflection::TypeGetObject(Type* type)
    {
        if (HaxSdk::IsMono())
        {
            MonoDomain* domain = mono::Domain::GetRoot();
            return (ReflectionType*)mono::Reflection::TypeGetObject(domain, (MonoType*)type);
        }
        return (ReflectionType*)il2cpp::Reflection::TypeGetObject((Il2CppType*)type);
    }

    ReflectionType* Object::GetType(Object* object)
    {
        Class* klass = Object::GetClass(object);
        return Reflection::TypeGetObject(Class::GetType(klass));
    }

    Object* Object::Box(Class* klass, void* data)
    {
        return HaxSdk::IsMono()
            ? (Object*)mono::Object::Box(mono::Domain::GetRoot(), (MonoClass*)klass, data)
            : (Object*)il2cpp::Object::Box((Il2CppClass*)klass, data);
    }

    uint32_t GCHandle::New(Object* obj, bool pinned)
    {
        return HaxSdk::IsMono()
            ? mono::GCHandle::New((MonoObject*)obj, pinned)
            : il2cpp::GCHandle::New((Il2CppObject*)obj, pinned);
    }

    uint32_t GCHandle::NewWeakRef(Object* obj, bool trackResurrection)
    {
        return HaxSdk::IsMono()
            ? mono::GCHandle::NewWeakRef((MonoObject*)obj, trackResurrection)
            : il2cpp::GCHandle::NewWeakRef((Il2CppObject*)obj, trackResurrection);
    }

    Object* GCHandle::GetTarget(uint32_t handle)
    {
        return HaxSdk::IsMono()
            ? (Object*)mono::GCHandle::GetTaget(handle)
            : (Object*)il2cpp::GCHandle::GetTaget(handle);
    }

    void GCHandle::Free(uint32_t handle)
    {
        HaxSdk::IsMono()
            ? mono::GCHandle::Free(handle)
            : il2cpp::GCHandle::Free(handle);
    }

    Class* ReflectionType::GetClass(ReflectionType* type)
    {
        return HaxSdk::IsMono()
            ? (Class*)mono::ReflectionType::GetClass((MonoReflectionType*)type)
            : (Class*)il2cpp::ReflectionType::GetClass((Il2CppReflectionType*)type);
    }

    Class* Object::GetClass(Object* object)
    {
        return HaxSdk::IsMono()
            ? (Class*)mono::Object::GetClass((MonoObject*)object)
            : (Class*)il2cpp::Object::GetClass((Il2CppObject*)object);
    }
}