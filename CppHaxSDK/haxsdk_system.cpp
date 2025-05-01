#include "haxsdk_system.h"

#include <format>

#include "haxsdk.h"
#include "backend/haxsdk_il2cpp.h"
#include "backend/haxsdk_mono.h"

template <>
System::Type typeof<int>()
{
    static auto type = System::Assembly::GetMscorlib().GetType("System", "Int32", true);
    return type;
}

namespace System
{
    bool Object::Equals(const Object& o) const
    {
        if (!m_ManagedPtr)
            throw NullReferenceException::New();

        return m_ManagedPtr == o.m_ManagedPtr;
    }

    Type Object::GetType() const
    {
        if (!m_ManagedPtr)
            throw NullReferenceException::New();

        if (HaxSdk::IsMono())
            return Type(Mono::ObjectGetType(m_MonoObj));
        
        return Type(Il2Cpp::ObjectGetType(m_Il2CppObj));
    }

    Object Object::Box(Type type, void* value)
    {
        return HaxSdk::IsMono() ? Object(Mono::BoxValue(type.m_MonoType, value)) : Object(Il2Cpp::BoxValue(type.m_Il2CppType, value));
    }

    void* Object::Unbox() const
    {
        if (!m_ManagedPtr)
            throw NullReferenceException::New();

        if (HaxSdk::IsMono())
            return Mono::Unbox(m_MonoObj);
        
        return Il2Cpp::Unbox(m_Il2CppObj);
    }

    Object Type::CreateInstance()
    {
        if (IsNull())
            throw System::NullReferenceException::New();

        return HaxSdk::IsMono() ? Object(Mono::NewObject(m_MonoType)) : Object(Il2Cpp::NewObject(m_Il2CppType));
    }

    FieldInfo Type::GetField(const char* name, bool doAssert)
    {
        if (!m_ManagedPtr)
            throw System::NullReferenceException::New();

        FieldInfo field = nullptr;
        if (HaxSdk::IsMono())
            field.m_MonoClassField = Mono::GetField(m_MonoType, name);
        else
            field.m_Il2CppFieldInfo = Il2Cpp::GetField(m_Il2CppType, name);
        
        if (doAssert)
            HAX_ASSERT(field.m_NativePtr, std::format("Field {} not found", name));

        return field;
    }

    MethodInfo Type::GetMethod(const char* name, const char* sig, bool doAssert)
    {
        if (!m_ManagedPtr)
            throw System::NullReferenceException::New();

        MethodInfo method = nullptr;
        if (HaxSdk::IsMono())
            method.m_MonoMethod = Mono::GetMethod(m_MonoType, name, sig);
        else
            method.m_Il2CppMethodInfo = Il2Cpp::GetMethod(m_Il2CppType, name, sig);

        if (doAssert)
            HAX_ASSERT(method.m_NativePtr, std::format("Method {} not found", name));
        return method;
    }

    Type String::TypeOf()
    {
        return Assembly::GetMscorlib().GetType("System", "String", true);
    }

    String String::New(const char* text)
    {
        return String(HaxSdk::IsMono() ? String(Mono::NewString(text)) : String(Il2Cpp::NewString(text)));
    }

    String String::Concat(const String& s1, const String& s2)
    {
        static HaxMethod method = TypeOf().GetMethod("Concat", "System.String(System.String,System.String");

        if (HaxSdk::IsMono())
        {
            if (method.m_Thunk)
                return method.Thunk<String, String, String>(s1, s2);
            return method.InvokeStatic<String, String, String>(s1, s2);
        }

        return method.Address<String, String, String>(s1, s2);
    }

    Int32 String::CompareTo(const String& s)
    {
        static HaxMethod method = TypeOf().GetMethod("CompareTo", "System.Int32(System.String)");

        if (!m_ManagedPtr)
            throw NullReferenceException::New();

        if (HaxSdk::IsMono())
        {
            if (method.m_Thunk)
                return method.Thunk<Int32, void*, void*>(m_ManagedPtr, s.m_ManagedPtr);
            return method.Invoke<Int32, void*, void*>(m_ManagedPtr, s.m_ManagedPtr);
        }

        return method.Address<Int32, void*, void*>(m_ManagedPtr, s.m_ManagedPtr);
    }

    wchar_t* String::GetRawStringData()
    {
        if (!m_ManagedPtr)
            throw NullReferenceException::New();

        return HaxSdk::IsMono() ? Mono::StringGetChars(m_MonoStr) : Il2Cpp::StringGetChars(m_Il2cppStr);
    }

    Int32 String::Length()
    {
        if (!m_ManagedPtr)
            throw NullReferenceException::New();

        return HaxSdk::IsMono() ? Mono::StringGetLength(m_MonoStr) : Il2Cpp::StringGetLength(m_Il2cppStr);
    }

    AppDomain AppDomain::GetDefaultDomain()
    {
        if (HaxSdk::IsMono())
            return AppDomain(Mono::GetDomain());

        return AppDomain(Il2Cpp::GetDomain());
    }

    Assembly AppDomain::Load(const char* name, bool doAssert)
    {
        if (!m_NativePtr)
            throw System::NullReferenceException::New();

        Assembly assembly(nullptr);
        if (HaxSdk::IsMono())
            assembly.m_MonoImage = Mono::LoadAssembly(m_MonoDomain, name);
        else
            assembly.m_Il2CppImage = Il2Cpp::LoadAssembly(m_Il2CppDomain, name);

        if (doAssert)
            HAX_ASSERT(assembly.m_NativePtr, std::format("Assembly {} not found", name));
        return assembly;
    }

    Assembly Assembly::GetMscorlib()
    {
        static Assembly cached(nullptr);

        if (!cached)
        {
            cached.m_NativePtr = HaxSdk::IsMono() 
                ? (void*)Mono::GetCoreLib() 
                : (void*)Il2Cpp::GetCoreLib();
        }

        return cached;
    }

    Assembly Assembly::GetUnityCoreLib()
    {

        static Assembly cached(nullptr);

        if (!cached)
        {
            cached.m_NativePtr = HaxSdk::IsMono()
                ? (void*)Mono::GetUnityCoreLib()
                : (void*)Il2Cpp::GetUnityCoreLib();
        }

        return cached;
    }

    AssemblyName Assembly::GetName()
    {
        if (!m_NativePtr)
            throw System::NullReferenceException::New();

        if (HaxSdk::IsMono())
            return AssemblyName(Mono::AssemblyGetName(m_MonoImage));

        return AssemblyName(Il2Cpp::AssemblyGetName(m_Il2CppImage));
    }

    Type Assembly::GetType(const char* nameSpace, const char* name, bool doAssert)
    {
        if (!m_NativePtr)
            throw System::NullReferenceException::New();

        Type type(nullptr);
        if (HaxSdk::IsMono())
            type.m_MonoType = Mono::AssemblyGetType(m_MonoImage, nameSpace, name);
        else
            type.m_Il2CppType = Il2Cpp::AssemblyGetType(m_Il2CppImage, nameSpace, name);

        if (doAssert)
            HAX_ASSERT(type.m_ManagedPtr, std::format("Class {}.{} not found", nameSpace, name));
        return type;
    }

    const char* AssemblyName::GetName()
    {
        if (!m_NativePtr)
            throw System::NullReferenceException::New();

        if (HaxSdk::IsMono())
            return Mono::AssemblyNameGetName(m_MonoAssemblyName);

        return Il2Cpp::AssemblyNameGetName(m_Il2CppAssemblyName);
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

    void FieldInfo::GetStaticValue(void* value)
    {
        if (!m_NativePtr)
            throw System::NullReferenceException::New();

        if (HaxSdk::IsMono())
            return Mono::GetStaticValue(m_MonoClassField, value);

        return Il2Cpp::GetStaticValue(m_Il2CppFieldInfo, value);
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

    void* MethodInfo::GetUnmanagedThunk() const
    {
        return HaxSdk::IsMono() ? Mono::GetUnmanagedThunk(m_MonoMethod) : nullptr;
    }

    void* MethodInfo::GetFunctionPointer() const
    {
        return HaxSdk::IsMono() ? Mono::GetFunctionPointer(m_MonoMethod) : Il2Cpp::GetFunctionPointer(m_Il2CppMethodInfo);
    }

    // Exceptions
    const char* Exception::GetMessageText() const
    {
        if (!m_ManagedPtr)
            throw NullReferenceException::New();

        if (HaxSdk::IsMono())
            return Mono::ExceptionGetMessage(m_MonoEx);

        return Il2Cpp::ExceptionGetMessage(m_Il2CppEx);
    }

    const char* Exception::GetStackTrace() const
    {
        if (!m_ManagedPtr)
            throw NullReferenceException::New();

        if (HaxSdk::IsMono())
            return  Mono::ExceptionGetStacktrace(m_MonoEx);

        return Il2Cpp::ExceptionGetStacktrace(m_Il2CppEx);
    }

    NullReferenceException NullReferenceException::New()
    {
        if (HaxSdk::IsMono())
            return NullReferenceException(Mono::NewObject(TypeOf().m_MonoType));

        return NullReferenceException(Il2Cpp::NewObject(TypeOf().m_Il2CppType));
    }

    ArgumentOutOfRangeException ArgumentOutOfRangeException::New()
    {
        if (HaxSdk::IsMono())
            return ArgumentOutOfRangeException(Mono::NewObject(TypeOf().m_MonoType));

        return ArgumentOutOfRangeException(Il2Cpp::NewObject(TypeOf().m_Il2CppType));
    }
}