#include "haxsdk_system.h"

#include <format>
#include <unordered_map>
#include <vector>

#include "haxsdk_assertion.h"
#include "haxsdk_logger.h"

struct FieldData
{
    System::FieldInfo& field;
    const char* assembly;
    const char* nameSpace;
    const char* klass;
    const char* name;
};

struct MethodData
{
    System::MethodInfo& method;
    const char* assembly;
    const char* nameSpace;
    const char* klass;
    const char* name;
    const char* signature;
};

struct TypeData
{
    System::Type& type;
    const char* assembly;
    const char* nameSpace;
    const char* name;
};

static std::unordered_map<backend::ReflectionType*, backend::Class*> g_TypeToClass;
static std::vector<FieldData> g_PreInitFields;
static std::vector<MethodData> g_PreInitMethods;
static std::vector<TypeData> g_PreInitTypes;
static bool g_Initialized;

namespace HaxSdk
{
    void InitSystem()
    {
        if (!g_Initialized)
        {
            g_Initialized = true;
            HaxSdk::AttachThread();

            typeof<int> = HaxSdk::GetMscorlib().GetType("System", "Int32", true);

            for (const auto& data : g_PreInitFields)
            {
                data.field = System::AppDomain::GetRootDomain().Load(data.assembly, true).GetType(data.nameSpace, data.klass, true).GetField(data.name, true);
                HAX_LOG_DEBUG("[PREFIELD] {}.{}.{} {}", data.nameSpace, data.klass, data.name, (void*)data.field.m_Field);
            }

            for (const auto& data : g_PreInitMethods)
            {
                data.method = System::AppDomain::GetRootDomain().Load(data.assembly, true).GetType(data.nameSpace, data.klass, true).GetMethod(data.name, data.signature, true);
                HAX_LOG_DEBUG("[PREMETHOD] {}.{}.{} {}", data.nameSpace, data.klass, data.name, (void*)data.method.m_Method);
            }

            for (const auto& data : g_PreInitTypes)
            {
                data.type = System::AppDomain::GetRootDomain().Load(data.assembly, true).GetType(data.nameSpace, data.name, true);
                HAX_LOG_DEBUG("[PRETYPE] {}.{} {}", data.nameSpace, data.name, (void*)data.type.m_Type);
            }
        }
    }

    System::Assembly GetMscorlib()
    {
        static System::Assembly mscorlib = System::AppDomain::GetRootDomain().Load("mscorlib");
        return mscorlib;
    }
}

namespace System
{
    Assembly AppDomain::Load(const char* name, bool doAssert)
    {
        if (!m_Domain)
            throw NullReferenceException::New();

        backend::Assembly* assembly = backend::Assembly::Load(name);
        backend::Image* image = assembly ? backend::Assembly::GetImage(assembly) : nullptr;
        if (doAssert)
            HAX_ASSERT_E(image, "Assembly {} not found", name);
        return Assembly(image);
    }

    Type Assembly::GetType(const char* nameSpace, const char* name, bool doAssert)
    {
        if (!m_Image)
            throw NullReferenceException::New();

        auto klass = backend::Class::FromName(m_Image, nameSpace, name);
        if (doAssert)
            HAX_ASSERT_E(klass, "Type {}.{} not found", nameSpace, name);
        
        if (klass)
        {
            auto type = backend::Class::GetType(klass);
            auto refType = backend::Reflection::TypeGetObject(type);
            g_TypeToClass[refType] = klass;
            return Type(refType);
        }
        return Type(nullptr);
    }

    Type Object::GetType()
    {
        if (!m_ManagedPtr)
            throw NullReferenceException::New();

        return Type(backend::Object::GetType(m_Object));
    }

    uint32_t GCHandle::GetTargetHandle(const Object& obj, GCHandleType type)
    {
        if (!obj.m_Object)
            return 0;
        return type == GCHandleType::Normal
            ? backend::GCHandle::New(obj.m_Object, false)
            : backend::GCHandle::NewWeakRef(obj.m_Object, false);
    }

    void GCHandle::Free()
    {
        if (m_Handle)
        {
            backend::GCHandle::Free(m_Handle);
            m_Handle = 0;
        }
    }

    const char* Exception::GetMessage()
    {
        if (!m_ManagedPtr)
            throw NullReferenceException::New();

        return backend::Exception::GetMessage(m_Exception);
    }

    NullReferenceException NullReferenceException::New()
    {
        return NullReferenceException(backend::Exception::GetNullReferenceException());
    }

    ArgumentOutOfRangeException ArgumentOutOfRangeException::New()
    {
        return ArgumentOutOfRangeException(backend::Exception::GetArgumentOutOfRangeException());
    }

    FieldInfo::FieldInfo(const char* assembly, const char* nameSpace, const char* klass, const char* name) : m_Field(nullptr)
    {
        if (!g_Initialized)
            g_PreInitFields.emplace_back(*this, assembly, nameSpace, klass, name);
    }

    MethodInfo::MethodInfo(const char* assembly, const char* nameSpace, const char* klass, const char* name, const char* signature)
    {
        if (!g_Initialized)
            g_PreInitMethods.emplace_back(*this, assembly, nameSpace, klass, name, signature);
    }

    Type::Type(const char* assembly, const char* nameSpace, const char* name) : Object(nullptr)
    {
        if (!g_Initialized)
            g_PreInitTypes.emplace_back(*this, assembly, nameSpace, name);
    }

    FieldInfo Type::GetField(const char* name, bool doAssert)
    {
        if (!m_ManagedPtr)
            throw NullReferenceException::New();

        auto field = backend::Class::GetFieldFromName(g_TypeToClass[m_Type], name);
        if (doAssert)
            HAX_ASSERT_E(field, "Field {} not found", name);
        return FieldInfo(field);
    }

    MethodInfo Type::GetMethod(const char* name, const char* signature, bool doAssert)
    {
        if (!m_ManagedPtr)
            throw NullReferenceException::New();

        auto method = backend::Class::GetMethodFromName(g_TypeToClass[m_Type], name, signature);
        if (doAssert)
            HAX_ASSERT_E(method, "Method {} not found", name);
        return MethodInfo(method);
    }
}