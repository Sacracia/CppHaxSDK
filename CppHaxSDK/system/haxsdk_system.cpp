#include "haxsdk_system.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef _WIN64
    #if __has_include("third_party/detours/x64/detours.h")
        #include "third_party/detours/x64/detours.h"
    #else
        #include "../third_party/detours/x64/detours.h"
    #endif
#else
    #if __has_include("third_party/detours/x86/detours.h")
        #include "third_party/detours/x86/detours.h"
    #else
        #include "../third_party/detours/x86/detours.h"
    #endif
#endif

#include "../haxsdk.h"
#include "../backend/haxsdk_il2cpp.h"
#include "../backend/haxsdk_mono.h"

namespace System
{
    Type Object::TypeOf()
    {
        static Type type = Assembly::GetMscorlib().GetType("System", "Object");
        return type;
    }

    bool Object::Equals(const Object& o) const
    {
        if (!ManagedPtr)
            throw NullReferenceException::New();

        return ManagedPtr == o.ManagedPtr;
    }

    Type Object::GetType()
    {
        if (!ManagedPtr)
            throw NullReferenceException::New();

        return HaxSdk::IsMono() ? Type(Mono::ObjectGetType(MonoObj)) : Type(Il2Cpp::ObjectGetType(Il2CppObj));
    }

    void* Object::Unbox()
    {
        if (!ManagedPtr)
            throw NullReferenceException::New();

        return HaxSdk::IsMono() ? Mono::Unbox(MonoObj) : Il2Cpp::Unbox(Il2CppObj);
    }

    Type String::TypeOf()
    {
        static Type type = Assembly::GetMscorlib().GetType("System", "String");
        return type;
    }

    String String::New(const char* text)
    {
        return HaxSdk::IsMono() ? String(Mono::NewString(text)) : String(Il2Cpp::NewString(text));
    }

    const char* Exception::GetMessageText() const
    {
        if (HaxSdk::IsMono())
            return Mono::ExceptionGetMessage(ToMonoException());

        return Il2Cpp::ExceptionGetMessage(ToIl2CppException());
    }

    const char* Exception::GetStackTrace() const
    {
        if (HaxSdk::IsMono())
            return  Mono::ExceptionGetStacktrace(ToMonoException());

        return Il2Cpp::ExceptionGetStacktrace(ToIl2CppException());
    }

    NullReferenceException NullReferenceException::New()
    {
        if (HaxSdk::IsMono())
            return NullReferenceException(Mono::NewNullReferenceException());

        return NullReferenceException(Il2Cpp::NewNullReferenceException());
    }

    void* Object::Unbox()
    {
        return HaxSdk::IsMono() ? Mono::Unbox(ToMonoObject()) : Il2Cpp::Unbox(ToIl2CppObject());
    }

    Type Object::GetType()
    {
        if (!m_ManagedObject)
            throw System::NullReferenceException::New();

        if (HaxSdk::IsMono())
            return Type(Mono::ObjectGetType(ToMonoObject()));
        
        return Type(Il2Cpp::ObjectGetType(ToIl2CppObject()));
    }

    AppDomain AppDomain::GetDefaultDomain()
    {
        if (HaxSdk::IsMono()) 
            return AppDomain(Mono::GetDomain());

        return AppDomain(Il2Cpp::GetDomain());
    }

    Assembly Assembly::GetMscorlib()
    {
        if (HaxSdk::IsMono())
            return Assembly(Mono::GetCoreLib());

        return Assembly(Il2Cpp::GetCoreLib());
    }

    Assembly Assembly::GetUnityCoreLib()
    {
        if (HaxSdk::IsMono())
            return Assembly(Mono::GetUnityCoreLib());

        return Assembly(Il2Cpp::GetUnityCoreLib());
    }

    Assembly AppDomain::Load(const char* name)
    {
        if (!m_NativePtr)
            throw System::NullReferenceException::New();

        if (HaxSdk::IsMono())
            return Assembly(Mono::LoadAssembly(m_MonoDomain, name));

        return Assembly(Il2Cpp::LoadAssembly(m_Il2CppDomain, name));
    }

    AssemblyName Assembly::GetName()
    {
        if (!m_NativePtr)
            throw System::NullReferenceException::New();

        if (HaxSdk::IsMono())
            return AssemblyName(Mono::AssemblyGetName(m_MonoImage));

        return AssemblyName(Il2Cpp::AssemblyGetName(m_Il2CppImage));
    }

    const char* AssemblyName::GetName()
    {
        if (!m_NativePtr)
            throw System::NullReferenceException::New();

        if (HaxSdk::IsMono())
            return Mono::AssemblyNameGetName(m_MonoAssemblyName);

        return Il2Cpp::AssemblyNameGetName(m_Il2CppAssemblyName);
    }

    Type Assembly::GetType(const char* nameSpace, const char* name)
    {
        if (!m_NativePtr)
            throw System::NullReferenceException::New();

        if (HaxSdk::IsMono())
            return Type(Mono::AssemblyGetType(m_MonoImage, nameSpace, name));

        return Type(Il2Cpp::AssemblyGetType(m_Il2CppImage, nameSpace, name));
    }

    FieldInfo Type::GetField(const char* name)
    {
        if (!m_NativePtr)
            throw System::NullReferenceException::New();

        if (HaxSdk::IsMono())
            return FieldInfo(Mono::GetField(m_MonoReflectionType, name));

        return FieldInfo(Il2Cpp::GetField(m_Il2CppReflectionType, name));
    }

    MethodInfo Type::GetMethod(const char* name, const char* sig)
    {
        if (!m_NativePtr)
            throw System::NullReferenceException::New();

        if (HaxSdk::IsMono())
            return MethodInfo(Mono::GetMethod(m_MonoReflectionType, name, sig));

        return MethodInfo(Il2Cpp::GetMethod(m_Il2CppReflectionType, name, sig));
    }

    int32_t FieldInfo::GetFieldOffset()
    {
        if (!m_NativePtr)
            throw System::NullReferenceException::New();

        if (HaxSdk::IsMono())
            Mono::GetFieldOffset(m_MonoClassField);

        return Il2Cpp::GetFieldOffset(m_Il2CppFieldInfo);
    }

    void* FieldInfo::GetStaticAddress()
    {
        if (!m_NativePtr)
            throw System::NullReferenceException::New();

        if (HaxSdk::IsMono())
            return Mono::GetStaticFieldAddress(m_MonoClassField);

        return Il2Cpp::GetStaticFieldAddress(m_Il2CppFieldInfo);
    }

    Object MethodInfo::Invoke(void* __this, void** args)
    {
        if (!m_NativePtr)
            throw System::NullReferenceException::New();

        if (HaxSdk::IsMono())
        {
            MonoException* ex = nullptr;
            MonoObject* res = Mono::Invoke(m_MonoMethod, __this, args, &ex);
            if (ex)
                throw System::Exception(ex);
            return Object(res);
        }

        Il2CppException* ex = nullptr;
        Il2CppObject* res = Il2Cpp::Invoke(m_Il2CppMethodInfo, __this, args, &ex);
        if (ex)
            throw System::Exception(ex);
        return Object(res);
    }

    void Hook(void** ptr, void* func)
    {
        DetourAttach(ptr, func);
    }

    void* MethodInfo::GetUnmanagedThunk()
    {
        return HaxSdk::IsMono() ? Mono::GetUnmanagedThunk(m_MonoMethod) : nullptr;
    }

    void* MethodInfo::GetFunctionPointer()
    {
        return HaxSdk::IsMono() ? Mono::GetFunctionPointer(m_MonoMethod) : Il2Cpp::GetFunctionPointer(m_Il2CppMethodInfo);
    }

    std::wostream& operator<<(std::wostream& os, const String& str)
    {
        return os << (HaxSdk::IsMono() ? Mono::StringGetChars(str.ToMonoString()) : Il2Cpp::StringGetChars(str.ToIl2CppString()));
    }

    std::ostream& operator<<(std::ostream& os, const String& str)
    {
        if (HaxSdk::IsMono())
        {
            wchar_t* wstr = Mono::StringGetChars(str.ToMonoString());
            int32_t length = Mono::StringGetLength(str.ToMonoString());
            return os << HaxSdk::ToUTF8(wstr, length);
        }

        wchar_t* wstr = Il2Cpp::StringGetChars(str.ToIl2CppString());
        int32_t length = Il2Cpp::StringGetLength(str.ToIl2CppString());
        return os << HaxSdk::ToUTF8(wstr, length);
    }

    template <typename T>
    static Type Array<T>::TypeOf()
    {
        static Type type = Assembly::GetMscorlib().GetType("System", "Array");
        return type;
    }

    template <typename T>
    void Array<T>::Clear(Array<T> array, int32_t index, int32_t length)
    {
        static HaxMethod<void(Array, int32_t, int32_t)> method = TypeOf().GetMethod("Clear", "System.Void(System.Array,System.Int32,System.Int32)");
        if (HaxSdk::IsMono())
            method.InvokeStatic(array, index, length);

        method.Adress(array, index, length);
    }

    template <typename T>
    void Array<T>::Sort(Array<T> array, int32_t index, int32_t length)
    {
        static HaxMethod<void(Array, int32_t, int32_t)> method = TypeOf().GetMethod("Sort", "System.Void(System.Array,System.Int32,System.Int32)");
        if (HaxSdk::IsMono())
            method.InvokeStatic(array, index, length);

        method.Adress(array, index, length);
    }
}