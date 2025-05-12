#include "haxsdk_system.h"

#include <format>
#include <unordered_map>
#include <vector>

#include "../backend/haxsdk_backend.h"
#include "../haxsdk_assertion.h"
#include "../haxsdk_logger.h"

struct FieldData
{
    System::FieldInfo&  m_Field;
    const char*         m_Assembly;
    const char*         m_Namespace;
    const char*         m_Class;
    const char*         m_Name;
};

struct MethodData
{
    System::MethodInfo& m_Method;
    const char* m_Assembly;
    const char* m_Namespace;
    const char* m_Class;
    const char* m_Name;
    const char* m_Signature;
};

struct TypeData
{
    System::Type& m_Type;
    const char* m_Assembly;
    const char* m_Namespace;
    const char* m_Name;
};

namespace vtables
{
    static unsafe::VTable* g_Int32;
}

static bool g_Initialized = false;
static std::vector<FieldData> g_PreInitFields;
static std::vector<MethodData> g_PreInitMethods;
static std::vector<TypeData> g_PreInitTypes;

namespace HaxSdk
{
    void InitSystem()
    {
        if (!g_Initialized)
        {
            g_Initialized = true;
            HaxSdk::InitBackend();

            vtables::g_Int32 = unsafe::Image::GetCorlib()->GetClass("System", "Int32")->GetVTable();

            for (auto& data : g_PreInitFields)
            {
                data.m_Field = System::AppDomain::GetCurrent().Load(data.m_Assembly).GetType(data.m_Namespace, data.m_Class).GetField(data.m_Name);
                HAX_LOG_DEBUG("[PREFIELD] {}__{} {}", data.m_Class, data.m_Name, (void*)data.m_Field.GetPointer());
            }

            for (auto& data : g_PreInitMethods)
            {
                data.m_Method = System::AppDomain::GetCurrent().Load(data.m_Assembly).GetType(data.m_Namespace, data.m_Class).GetMethod(data.m_Name, data.m_Signature);
                HAX_LOG_DEBUG("[PREMETHOD] {}__{} {}", data.m_Class, data.m_Name, (void*)data.m_Method.m_Pointer);
            }

            for (auto& data : g_PreInitTypes)
            {
                data.m_Type = System::AppDomain::GetCurrent().Load(data.m_Assembly).GetType(data.m_Namespace, data.m_Name);
                HAX_LOG_DEBUG("[RRETYPE] {}.{} {}", data.m_Namespace, data.m_Name, (void*)data.m_Type.GetPointer());
            }
        }
    }

    void ThrowNullRef()
    {
        throw System::NullReferenceException::New();
    }

    void ThrowOutOfRange()
    {
        throw System::ArgumentOutOfRangeException::New();
    }
}

namespace System
{
    Type Object::GetType()
    {
        CHECK_NULL(); 
        return Type(m_Pointer->GetClass()->GetType()->GetReflectionType());
    }

    Type Assembly::GetType(const char* nameSpace, const char* name, bool doAssert)
    {
        CHECK_NULL();
        unsafe::Class* klass = m_Pointer->GetClass(nameSpace, name, doAssert);
        return klass ? Type(klass->GetType()->CreateReflectionType()) : Type((unsafe::Object*)nullptr);
    }

    FieldInfo::FieldInfo(const char* assembly, const char* nameSpace, const char* klass, const char* name)
    {
        if (!g_Initialized)
            g_PreInitFields.emplace_back(*this, assembly, nameSpace, klass, name);
    }

    void* FieldInfo::GetValuePtr(const Object& __this)
    {
        CHECK_NULL();
        if (!IsStatic() && !__this)
            throw TargetException::New(String::New("Non-static field requires a target"));
        return m_Pointer->GetValuePtr(__this.GetPointer());
    }

    MethodInfo Type::GetMethod(const char* name, const char* sig, bool doAssert)
    { 
        CHECK_NULL(); 
        return MethodInfo(GetPointer()->GetType()->GetClass()->GetMethod(name, sig, doAssert)); 
    }

    MethodInfo::MethodInfo(const char* assembly, const char* nameSpace, const char* klass, const char* name, const char* sig)
    {
        if (!g_Initialized)
            g_PreInitMethods.emplace_back(*this, assembly, nameSpace, klass, name, sig);
    }

    Type::Type(const char* assembly, const char* nameSpace, const char* name)
    {
        if (!g_Initialized)
            g_PreInitTypes.emplace_back(*this, assembly, nameSpace, name);
    }

    Int32_Boxed::Int32_Boxed(int v) : Object(vtables::g_Int32), m_Value(v)
    {

    }
}