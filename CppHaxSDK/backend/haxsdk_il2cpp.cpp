#include "haxsdk_il2cpp.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <format>

#include "../haxsdk.h"

#define GET_BACKEND_API(n)      n = (t ## _ ## n)(GetProcAddress(handle, #n));\
                                HaxSdk::Log(std::format("{} {}", #n, (void*)n));\
                                HAX_ASSERT(n, #n)
#define GET_BACKEND_API_OPT(n)  n = (t ## _ ## n)(GetProcAddress(handle, #n));\
                                HaxSdk::Log(std::format("{} {}", #n, (void*)n))

static void GetMethodSignature(Il2CppMethod* method, char* buff, size_t buffSize);
static Il2CppAssembly* FindAssembly(const char* assembly);

void Il2Cpp::Initialize()
{
    HMODULE handle = (HMODULE)HaxSdk::GetGlobals().BackendHandle;
    GET_BACKEND_API(il2cpp_object_new);
    GET_BACKEND_API(il2cpp_object_unbox);
    GET_BACKEND_API(il2cpp_runtime_object_init);
    GET_BACKEND_API(il2cpp_domain_get);
    GET_BACKEND_API(il2cpp_domain_get_assemblies);
    GET_BACKEND_API(il2cpp_thread_attach);
    GET_BACKEND_API(il2cpp_assembly_get_image);
    GET_BACKEND_API(il2cpp_class_from_name);
    GET_BACKEND_API(il2cpp_class_get_type);
    GET_BACKEND_API(il2cpp_class_get_methods);
    GET_BACKEND_API(il2cpp_type_get_object);
    GET_BACKEND_API(il2cpp_type_get_name);
    GET_BACKEND_API(il2cpp_class_get_field_from_name);
    GET_BACKEND_API(il2cpp_runtime_invoke);
    GET_BACKEND_API(il2cpp_method_get_param_count);
    GET_BACKEND_API(il2cpp_method_get_param);
    GET_BACKEND_API(il2cpp_method_get_class);
    GET_BACKEND_API(il2cpp_method_get_name);
    GET_BACKEND_API(il2cpp_method_get_return_type);
    GET_BACKEND_API(il2cpp_string_new);
    GET_BACKEND_API(il2cpp_field_static_get_value);
    GET_BACKEND_API(il2cpp_field_static_set_value);
    GET_BACKEND_API_OPT(il2cpp_class_get_static_field_data);
    GET_BACKEND_API(il2cpp_thread_detach);
}

void Il2Cpp::AttachThread()
{
    il2cpp_thread_attach(il2cpp_domain_get());
}

bool Il2Cpp::AssemblyExists(const char* assembly, Il2CppAssembly** out)
{
    Il2CppAssembly* il2cppAssembly = FindAssembly(assembly);
    if (il2cppAssembly && out)
        *out = il2cppAssembly;
    return il2cppAssembly;
}

int32_t Il2Cpp::FindField(const char* assembly, const char* nameSpace, const char* klass, const char* field)
{
    Il2CppAssembly* il2cppAssembly = FindAssembly(assembly);
    HAX_ASSERT(il2cppAssembly, std::format("Assembly {} not found", assembly).c_str());

    Il2CppClass* il2cppClass = il2cpp_class_from_name(il2cppAssembly->Image, nameSpace, klass);
    HAX_ASSERT(il2cppClass, std::format("Class {}.{} not found in {}", nameSpace, klass, assembly).c_str());

    Il2CppField* il2cppField = il2cpp_class_get_field_from_name(il2cppClass, field);
    HAX_ASSERT(il2cppField, std::format("Field {} not found in {}.{}", field, nameSpace, klass).c_str());

    return il2cppField->Offset;
}

void* Il2Cpp::FindStaticField(const char* assembly, const char* nameSpace, const char* klass, const char* field)
{
    Il2CppAssembly* il2cppAssembly = FindAssembly(assembly);
    HAX_ASSERT(il2cppAssembly, std::format("Assembly {} not found", assembly).c_str());

    Il2CppClass* il2cppClass = il2cpp_class_from_name(il2cppAssembly->Image, nameSpace, klass);
    HAX_ASSERT(il2cppClass, std::format("Class {}.{} not found in {}", nameSpace, klass, assembly).c_str());

    Il2CppField* il2cppField = il2cpp_class_get_field_from_name(il2cppClass, field);
    HAX_ASSERT(il2cppField, std::format("Field {} not found in {}.{}", field, nameSpace, klass).c_str());

    return (char*)il2cppClass->StaticFields + il2cppField->Offset;
}

MethodParams Il2Cpp::FindMethod(const char* assembly, const char* nameSpace, const char* klass, const char* method, const char* sig)
{
    Il2CppAssembly* il2cppAssembly = FindAssembly(assembly);
    HAX_ASSERT(il2cppAssembly, std::format("Assembly {} not found", assembly).c_str());

    Il2CppClass* il2cppClass = il2cpp_class_from_name(il2cppAssembly->Image, nameSpace, klass);
    HAX_ASSERT(il2cppClass, std::format("Class {}.{} not found in {}", nameSpace, klass, assembly).c_str());

    void* iter = nullptr;
    Il2CppMethod* il2cppMethod = nullptr;
    while (il2cppMethod = il2cpp_class_get_methods(il2cppClass, &iter))
    {
        const char* methodName = il2cpp_method_get_name(il2cppMethod);
        if (strcmp(methodName, method) != 0)
            continue;

        if (!sig || sig[0] == '\0')
            break;

        char buff[256] = {0};
        GetMethodSignature(il2cppMethod, buff, sizeof(buff));
        if (strcmp(buff, sig) == 0)
            break;
    }

    HAX_ASSERT(il2cppMethod, std::format("Method {} not found in {}.{}", method, nameSpace, klass).c_str());
    MethodParams params;
    params.BackendMethod = (void*)il2cppMethod;
    params.Address = params.Thunk = il2cppMethod->Ptr;
    return params;
}

static void GetMethodSignature(Il2CppMethod* method, char* buff, size_t buffSize)
{
    strcpy_s(buff, buffSize, il2cpp_type_get_name(il2cpp_method_get_return_type(method)));
    strcat_s(buff, buffSize, "(");
    uint32_t nParams = il2cpp_method_get_param_count(method);
    if (nParams > 0) 
    {
        strcat_s(buff, buffSize, il2cpp_type_get_name(il2cpp_method_get_param(method, 0)));
        for (uint32_t i = 1; i < nParams; ++i) 
        {
            strcat_s(buff, buffSize, ",");
            strcat_s(buff, buffSize, il2cpp_type_get_name(il2cpp_method_get_param(method, i)));
        }
    }
    strcat_s(buff, buffSize, ")");
}

static Il2CppAssembly* FindAssembly(const char* assembly)
{
    Il2CppDomain* il2cppDomain = il2cpp_domain_get();
    size_t nDomains = 0;
    Il2CppAssembly** assemblyIter = il2cpp_domain_get_assemblies(il2cppDomain, &nDomains);
    for (size_t i = 0; i < nDomains; ++i)
    {
        if (strcmp(assemblyIter[i]->Name, assembly) == 0)
            return assemblyIter[i];
    }
    return nullptr;
}