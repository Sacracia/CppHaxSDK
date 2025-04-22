#include "haxsdk_backend.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <format>
#include <unordered_map>
#include <stdexcept>

#ifdef _WIN64
#include "third_party/detours/x64/detours.h"
#else
#include "third_party/detours/x86/detours.h"
#endif

#include "haxsdk.h"

#define GET_BACKEND_API(n)      n = (t ## _ ## n)(GetProcAddress(handle, #n));\
                                HaxSdk::Log(std::format("{} {}", #n, (void*)n));\
                                HAX_ASSERT(n, #n)
#define GET_BACKEND_API_OPT(n)  n = (t ## _ ## n)(GetProcAddress(handle, #n));\
                                HaxSdk::Log(std::format("{} {}", #n, (void*)n))

struct MonoAssembly;
struct MonoClass;
struct MonoDomain;
struct MonoType;
struct MonoImage;
struct MonoMethod;
struct MonoObject;
struct MonoString;
struct MonoThread;
struct MonoVTable;
struct MonoMethodSignature;
struct MonoException;

struct MonoField
{
    MonoType*   MonoType;
    const char* Name;
    MonoClass*  Parent;
    int32_t     Offset; // In Mono docs it's int
};

struct Il2CppImage;
struct Il2CppDomain;
struct Il2CppThread;
struct Il2CppString;
struct Il2CppException;
struct Il2CppVTable;
struct MonitorData;

struct Il2CppType 
{
    void*               Data;
    unsigned int        Attrs : 16;
    int                 Type : 8;
    unsigned int        NumMods : 5;
    unsigned int        Byref : 1;
    unsigned int        Pinned : 1;
    unsigned int        Valuetype : 1;
};

struct Il2CppClass 
{
    Il2CppImage*        Image;
    void*               GcDesc;
    const char*         Name;
    const char*         NameSpace;
    Il2CppType          BvalArg;
    Il2CppType          ThisArg;
    void*               __space[15];
    void*               StaticFields;
};

struct Il2CppAssembly
{
    Il2CppImage*        Image;
    uint32_t            Token;
    int32_t             ReferencedAssemblyStart;
    int32_t             ReferencedAssemblyCount;
    const char*         Name; // Actually it's Il2CppAssembly but we need only the first field of Il2CppAssembly - name.
};

struct Il2CppField
{
    const char*         Name;
    const Il2CppType*   Type;
    Il2CppClass*        Parent;
    int32_t             Offset; // If offset is -1, then it's thread static
    uint32_t            Token;
};

struct Il2CppMethod {
    void* Pointer;
    // <...>
};

struct Il2CppObject
{
    union
    {
        Il2CppClass* klass;
        Il2CppVTable* vtable;
    };
    MonitorData* monitor;
};

using GFunc                                 = void(__cdecl*)(void* data, void* user_data);
using t_mono_get_root_domain                = MonoDomain* (*)();
using t_mono_domain_assembly_open           = MonoAssembly* (*)(MonoDomain*, const char*);
using t_mono_thread_attach                  = MonoThread* (*)(MonoDomain*);
using t_mono_domain_get                     = MonoDomain* (*)();
using t_mono_string_new                     = MonoString* (*)(MonoDomain*, const char*);
using t_mono_assembly_get_image             = MonoImage* (*)(MonoAssembly*);
using t_mono_assembly_foreach               = void (*)(GFunc, void*);
using t_mono_class_from_name                = MonoClass* (*)(MonoImage*, const char*, const char*);
using t_mono_class_get_methods              = MonoMethod* (*)(MonoClass*, void**);
using t_mono_class_get_field_from_name      = MonoField* (*)(MonoClass*, const char*);
using t_mono_class_vtable                   = MonoVTable* (*)(MonoDomain*, MonoClass*);
using t_mono_class_get_namespace            = const char* (*)(MonoClass*);
using t_mono_class_get_name                 = const char* (*)(MonoClass*);
using t_mono_object_get_class               = MonoClass* (*)(MonoObject*);
using t_mono_class_get_type                 = MonoType* (*)(MonoClass*);
using t_mono_object_get_class               = MonoClass* (*)(MonoObject*);
using t_mono_method_signature               = MonoMethodSignature* (*)(MonoMethod*);
using t_mono_method_get_name                = const char* (*)(MonoMethod*);
using t_mono_compile_method                 = void* (*)(MonoMethod*);
using t_mono_jit_info_table_find            = void* (*)(MonoDomain*, void*);
using t_mono_jit_info_get_method            = MonoMethod* (*)(void*);
using t_mono_runtime_invoke                 = MonoObject* (*)(MonoMethod*, void*, void**, MonoObject**);
using t_mono_print_unhandled_exception      = void (*)(MonoObject*);
using t_mono_object_unbox                   = void* (*)(MonoObject*);
using t_mono_vtable_get_static_field_data   = void* (*)(MonoVTable*);
using t_mono_type_get_object                = MonoType* (*)(MonoDomain*, MonoType*);
using t_mono_object_new                     = MonoObject * (*)(MonoDomain*, MonoClass*);
using t_mono_runtime_object_init            = void (*)(MonoObject*);
using t_mono_signature_get_return_type      = MonoType* (*)(MonoMethodSignature*);
using t_mono_type_get_name                  = char* (*)(MonoType*);
using t_mono_signature_get_param_count      = uint32_t (*)(MonoMethodSignature*);
using t_mono_signature_get_params           = MonoType* (*)(MonoMethodSignature*, void**);
using t_mono_method_get_unmanaged_thunk     = void* (*)(MonoMethod*);
using t_mono_string_to_utf8                 = char* (*)(MonoString*);
using t_mono_field_static_get_value         = void (*)(void* pVtable, MonoField* pField, void* pValue);
using t_mono_field_static_set_value         = void (*)(void* pVtable, MonoField* pField, void* pValue);
using t_mono_thread_detach                  = void (*)(MonoThread*);
using t_mono_image_get_name                 = const char* (*)(MonoImage*);
using t_mono_lookup_internal_call           = void* (*)(void*);
using t_mono_object_to_string               = MonoString* (*)(MonoObject*);
using t_mono_type_is_void                   = bool (*)(MonoType* type);
using t_mono_thread_current                 = MonoThread* (*)();

using t_il2cpp_object_new                   = Il2CppObject* (*)(Il2CppClass* pClass);
using t_il2cpp_object_unbox                 = void* (*)(Il2CppObject* obj);
using t_il2cpp_runtime_object_init          = void (*)(Il2CppObject* obj);
using t_il2cpp_domain_get                   = Il2CppDomain* (*)();
using t_il2cpp_domain_get_assemblies        = Il2CppAssembly* *(*)(Il2CppDomain* domain, size_t* size);
using t_il2cpp_thread_attach                = Il2CppThread* (*)(Il2CppDomain* domain);
using t_il2cpp_assembly_get_image           = Il2CppImage* (*)(Il2CppAssembly* assembly);
using t_il2cpp_class_from_name              = Il2CppClass* (*)(Il2CppImage* image, const char* namespaze, const char* name);
using t_il2cpp_class_get_type               = Il2CppType* (*)(Il2CppClass* pClass);
using t_il2cpp_class_get_methods            = Il2CppMethod* (*)(Il2CppClass* pClass, void** ppIter);
using t_il2cpp_type_get_object              = Il2CppType* (*)(Il2CppType* type);
using t_il2cpp_type_get_name                = const char* (*)(Il2CppType* type);
using t_il2cpp_class_get_field_from_name    = Il2CppField* (*)(Il2CppClass* pClass, const char* name);
using t_il2cpp_runtime_invoke               = Il2CppObject* (*)(Il2CppMethod* pMethod, void* obj, void** params, Il2CppException** exc);
using t_il2cpp_method_get_param_count       = int32_t(*)(Il2CppMethod* pMethod);
using t_il2cpp_method_get_param             = Il2CppType* (*)(Il2CppMethod* pMethod, uint32_t index);
using t_il2cpp_method_get_class             = Il2CppClass* (*)(Il2CppMethod* pMethod);
using t_il2cpp_method_get_name              = const char* (*)(Il2CppMethod* pMethod);
using t_il2cpp_method_get_return_type       = Il2CppType* (*)(Il2CppMethod* pMethod);
using t_il2cpp_string_new                   = Il2CppString * (*)(const char* str);
using t_il2cpp_class_get_static_field_data  = void* (*)(Il2CppClass*);
using t_il2cpp_field_static_get_value       = void (*)(Il2CppField* pField, void* pValue);
using t_il2cpp_field_static_set_value       = void (*)(Il2CppField* pField, void* pValue);
using t_il2cpp_thread_detach                = void (*)(Il2CppThread*);
using t_il2cpp_image_get_name               = const char* (*)(Il2CppImage*);

static t_mono_get_root_domain               mono_get_root_domain;
static t_mono_domain_assembly_open          mono_domain_assembly_open;
static t_mono_thread_attach                 mono_thread_attach;
static t_mono_domain_get                    mono_domain_get;
static t_mono_string_new                    mono_string_new;
static t_mono_assembly_get_image            mono_assembly_get_image;
static t_mono_assembly_foreach              mono_assembly_foreach;
static t_mono_class_from_name               mono_class_from_name;
static t_mono_class_get_methods             mono_class_get_methods;
static t_mono_class_get_field_from_name     mono_class_get_field_from_name;
static t_mono_class_get_namespace           mono_class_get_namespace;
static t_mono_class_get_name                mono_class_get_name;
static t_mono_class_vtable                  mono_class_vtable;
static t_mono_object_get_class              mono_object_get_class;
static t_mono_class_get_type                mono_class_get_type;
static t_mono_method_signature              mono_method_signature;
static t_mono_method_get_name               mono_method_get_name;
static t_mono_compile_method                mono_compile_method;
static t_mono_jit_info_table_find           mono_jit_info_table_find;
static t_mono_jit_info_get_method           mono_jit_info_get_method;
static t_mono_runtime_invoke                mono_runtime_invoke;
static t_mono_object_unbox                  mono_object_unbox;
static t_mono_vtable_get_static_field_data  mono_vtable_get_static_field_data;
static t_mono_type_get_object               mono_type_get_object;
static t_mono_object_new                    mono_object_new;
static t_mono_runtime_object_init           mono_runtime_object_init;
static t_mono_signature_get_return_type     mono_signature_get_return_type;
static t_mono_type_get_name                 mono_type_get_name;
static t_mono_signature_get_param_count     mono_signature_get_param_count;
static t_mono_signature_get_params          mono_signature_get_params;
static t_mono_method_get_unmanaged_thunk    mono_method_get_unmanaged_thunk;
static t_mono_string_to_utf8                mono_string_to_utf8;
static t_mono_field_static_get_value        mono_field_static_get_value;
static t_mono_field_static_set_value        mono_field_static_set_value;
static t_mono_thread_detach                 mono_thread_detach;
static t_mono_image_get_name                mono_image_get_name;
static t_mono_lookup_internal_call          mono_lookup_internal_call;
static t_mono_object_to_string              mono_object_to_string;
static t_mono_type_is_void                  mono_type_is_void;
static t_mono_thread_current                mono_thread_current;

static t_il2cpp_object_new                  il2cpp_object_new;
static t_il2cpp_object_unbox                il2cpp_object_unbox;
static t_il2cpp_runtime_object_init         il2cpp_runtime_object_init;
static t_il2cpp_domain_get                  il2cpp_domain_get;
static t_il2cpp_domain_get_assemblies       il2cpp_domain_get_assemblies;
static t_il2cpp_thread_attach               il2cpp_thread_attach;
static t_il2cpp_assembly_get_image          il2cpp_assembly_get_image;
static t_il2cpp_class_from_name             il2cpp_class_from_name;
static t_il2cpp_class_get_type              il2cpp_class_get_type;
static t_il2cpp_class_get_methods           il2cpp_class_get_methods;
static t_il2cpp_type_get_object             il2cpp_type_get_object;
static t_il2cpp_type_get_name               il2cpp_type_get_name;
static t_il2cpp_class_get_field_from_name   il2cpp_class_get_field_from_name;
static t_il2cpp_runtime_invoke              il2cpp_runtime_invoke;
static t_il2cpp_method_get_param_count      il2cpp_method_get_param_count;
static t_il2cpp_method_get_param            il2cpp_method_get_param;
static t_il2cpp_method_get_class            il2cpp_method_get_class;
static t_il2cpp_method_get_name             il2cpp_method_get_name;
static t_il2cpp_method_get_return_type      il2cpp_method_get_return_type;
static t_il2cpp_string_new                  il2cpp_string_new;
static t_il2cpp_class_get_static_field_data il2cpp_class_get_static_field_data;
static t_il2cpp_field_static_get_value      il2cpp_field_static_get_value;
static t_il2cpp_field_static_set_value      il2cpp_field_static_set_value;
static t_il2cpp_thread_detach               il2cpp_thread_detach;
static t_il2cpp_image_get_name              il2cpp_image_get_name;

static std::unordered_map<uint64_t, void*> g_CachedAssemblies;

constexpr static uint64_t HashString(const char* str) {
    uint64_t hash = 14695981039346656037ULL; // FNV-1a offset basis
    while (*str) {
        hash ^= static_cast<uint64_t>(*str);
        hash *= 1099511628211ULL; // FNV-1a prime
        str++;
    }
    return hash;
}

// Комбинирует хеши нескольких строк (аналог boost::hash_combine)
constexpr static uint64_t HashStrings(const char* str1, const char* str2, const char* str3 = nullptr, const char* str4 = nullptr) {
    uint64_t seed = HashString(str1);

    // Комбинируем хеши с битовыми сдвигами и XOR
    auto combine = [&seed](const char* str) {
        if (str) {
            uint64_t hash = HashString(str);
            seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };

    combine(str2);
    combine(str3);
    combine(str4);

    return seed;
}

namespace Mono
{
    static void AttachThread()
    {
        mono_thread_attach(mono_get_root_domain());
    }

    static void Initialize()
    {
        HMODULE handle = (HMODULE)HaxSdk::GetGlobals().BackendHandle;
        GET_BACKEND_API(mono_assembly_foreach);
        GET_BACKEND_API(mono_assembly_get_image);
        GET_BACKEND_API(mono_class_from_name);
        GET_BACKEND_API(mono_class_get_field_from_name);
        GET_BACKEND_API(mono_class_get_methods);
        GET_BACKEND_API(mono_class_get_name);
        GET_BACKEND_API(mono_class_get_namespace);
        GET_BACKEND_API(mono_class_get_type);
        GET_BACKEND_API(mono_class_vtable);
        GET_BACKEND_API(mono_compile_method);
        GET_BACKEND_API(mono_domain_assembly_open);
        GET_BACKEND_API(mono_domain_get);
        GET_BACKEND_API(mono_field_static_get_value);
        GET_BACKEND_API(mono_field_static_set_value);
        GET_BACKEND_API(mono_get_root_domain);
        GET_BACKEND_API(mono_image_get_name);
        GET_BACKEND_API(mono_jit_info_get_method);
        GET_BACKEND_API(mono_jit_info_table_find);
        GET_BACKEND_API(mono_lookup_internal_call);
        GET_BACKEND_API(mono_method_get_name);
        GET_BACKEND_API(mono_method_signature);
        GET_BACKEND_API(mono_object_get_class);
        GET_BACKEND_API(mono_object_new);
        GET_BACKEND_API(mono_object_unbox);
        GET_BACKEND_API(mono_runtime_invoke);
        GET_BACKEND_API(mono_runtime_object_init);
        GET_BACKEND_API(mono_signature_get_param_count);
        GET_BACKEND_API(mono_signature_get_params);
        GET_BACKEND_API(mono_signature_get_return_type);
        GET_BACKEND_API(mono_string_new);
        GET_BACKEND_API(mono_string_to_utf8);
        GET_BACKEND_API(mono_thread_attach);
        GET_BACKEND_API(mono_thread_detach);
        GET_BACKEND_API(mono_type_get_name);
        GET_BACKEND_API(mono_type_get_object);
        GET_BACKEND_API(mono_vtable_get_static_field_data);
        GET_BACKEND_API(mono_thread_current);
        GET_BACKEND_API_OPT(mono_method_get_unmanaged_thunk);
        GET_BACKEND_API_OPT(mono_object_to_string);
        GET_BACKEND_API_OPT(mono_type_is_void);
    }

    static void _cdecl AssemblyEnumerator(void* assembly, void* params)
    {
        MonoAssembly* monoAssembly = (MonoAssembly*)assembly;
        auto converted = (std::pair<const char*, MonoAssembly**>*)params;
        MonoImage* monoImage = mono_assembly_get_image(monoAssembly);
        const char* assemblyName = mono_image_get_name(monoImage);
        if (strcmp(assemblyName, converted->first) == 0)
            *converted->second = monoAssembly;
    }

    static MonoAssembly* FindAssembly(const char* assembly)
    {
        uint64_t hash = HashString(assembly);
        if (g_CachedAssemblies.contains(hash))
        {
            HaxSdk::Log(std::format("Using cached assembly {}", assembly));
            return (MonoAssembly*)g_CachedAssemblies[hash];
        }

        MonoDomain* domain = mono_get_root_domain();
        MonoAssembly* monoAssembly = nullptr;
        auto params = std::make_pair(assembly, &monoAssembly);
        mono_assembly_foreach(AssemblyEnumerator, &params);

        if (monoAssembly)
            g_CachedAssemblies[hash] = monoAssembly;
        return monoAssembly;
    }

    static MonoClass* FindClass(MonoAssembly* assembly, const char* nameSpace, const char* klass)
    {
        if (!assembly)
            return nullptr;
        return mono_class_from_name(mono_assembly_get_image(assembly), nameSpace, klass);
    }

    static int32_t FindField(MonoClass* klass, const char* field)
    {
        if (!klass)
            return -1;

        MonoField* monoField = mono_class_get_field_from_name(klass, field);
        return monoField ? monoField->Offset : -1;
    }

    static void* FindStaticField(MonoClass* klass, const char* field)
    {
        int32_t offset = FindField(klass, field);
        if (offset < 0)
            return nullptr;

        MonoVTable* vtable = mono_class_vtable(mono_get_root_domain(), klass);
        void* data = mono_vtable_get_static_field_data(vtable);
        return (char*)data + offset;
    }

    static void GetMethodSignature(MonoMethod* method, char* buff, size_t buffSize)
    {
        MonoMethodSignature* signature = mono_method_signature(method);
        MonoType* returnType = mono_signature_get_return_type(signature);
        strcpy_s(buff, buffSize, mono_type_get_name(returnType));
        strcat_s(buff, buffSize, "(");
        uint32_t nParams = mono_signature_get_param_count(signature);
        if (nParams > 0) 
        {
            void* iter = nullptr;
            strcat_s(buff, buffSize, mono_type_get_name(mono_signature_get_params(signature, &iter)));
            while (MonoType* paramType = mono_signature_get_params(signature, &iter)) 
            {
                strcat_s(buff, buffSize, ",");
                strcat_s(buff, buffSize, mono_type_get_name(paramType));
            }
        }
        strcat_s(buff, buffSize, ")");
    }

    static MonoMethod* FindMethod(MonoClass* klass, const char* name, const char* signature)
    {
        if (!klass)
            return nullptr;

        void* iter = nullptr;
        while (MonoMethod* method = mono_class_get_methods(klass, &iter))
        {
            const char* methodName = mono_method_get_name(method);
            if (strcmp(methodName, name) == 0)
            {
                if (!signature || !signature[0])
                    return method;

                char buff[256] = {0};
                GetMethodSignature(method, buff, sizeof(buff));
                if (strcmp(buff, signature) == 0)
                    return method;
            }
        }
        return nullptr;
    }
}

namespace Il2Cpp
{
    static void AttachThread()
    {
        il2cpp_thread_attach(il2cpp_domain_get());
    }

    static void Initialize()
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
        GET_BACKEND_API(il2cpp_image_get_name);
    }

    static Il2CppAssembly* FindAssembly(const char* assembly)
    {
        uint64_t hash = HashString(assembly);
        if (g_CachedAssemblies.contains(hash))
        {
            HaxSdk::Log(std::format("Using cached assembly {}", assembly));
            return (Il2CppAssembly*)g_CachedAssemblies[hash];
        }

        Il2CppDomain* il2cppDomain = il2cpp_domain_get();
        size_t nDomains = 0;
        Il2CppAssembly** assemblyIter = il2cpp_domain_get_assemblies(il2cppDomain, &nDomains);
        for (size_t i = 0; i < nDomains; ++i)
        {
            Il2CppAssembly* il2cppAssembly = assemblyIter[i];
            if (strcmp(il2cppAssembly->Name, assembly) == 0)
            {
                g_CachedAssemblies[hash] = il2cppAssembly;
                return il2cppAssembly;
            }
        }

        return nullptr;
    }

    static Il2CppClass* FindClass(Il2CppAssembly* assembly, const char* nameSpace, const char* klass)
    {
        //uint64_t classHash = HashStrings(assembly, nameSpace, klass);
        //if (g_CachedClasses.contains(classHash))
        //{
        //    HaxSdk::Log(std::format("Using cached class {}.{}", nameSpace, klass));
        //    return (Il2CppClass*)g_CachedClasses[classHash];
        //}
        if (!assembly)
            return nullptr;
        return il2cpp_class_from_name(assembly->Image, nameSpace, klass);
    }

    static int32_t FindField(Il2CppClass* klass, const char* field)
    {
        if (!klass) 
            return -1;

        Il2CppField* il2cppField = il2cpp_class_get_field_from_name(klass, field);
        return il2cppField ? il2cppField->Offset : -1;
    }

    static void* FindStaticField(Il2CppClass* klass, const char* field)
    {
        int32_t offset = FindField(klass, field);
        if (offset < 0)
            return nullptr;

        return (char*)klass->StaticFields + offset;
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

    static Il2CppMethod* FindMethod(Il2CppClass* klass, const char* name, const char* signature)
    {
        if (!klass)
            return nullptr;

        void* iter = nullptr;
        while (Il2CppMethod* method = il2cpp_class_get_methods(klass, &iter))
        {
            const char* methodName = il2cpp_method_get_name(method);
            if (strcmp(methodName, name) == 0)
            {
                if (!signature || !signature[0])
                    return method;

                char buff[256] = {0};
                GetMethodSignature(method, buff, sizeof(buff));
                if (strcmp(buff, signature) == 0)
                    return method;
            }
        }
        return nullptr;
    }
}

namespace HaxSdk
{
    void AttachThread()
    {
        HaxSdk::GetGlobals().Backend == HaxBackend_Mono ? Mono::AttachThread() : Il2Cpp::AttachThread();
    }

    void InitializeBackend()
    {
        HaxSdk::GetGlobals().Backend == HaxBackend_Mono ? Mono::Initialize() : Il2Cpp::Initialize();
    }
}

bool HaxAssembly::Exists(const char* assembly, HaxAssembly* out)
{
    void* result = HaxSdk::GetGlobals().Backend == HaxBackend_Mono ? (void*)Mono::FindAssembly(assembly) : (void*)Il2Cpp::FindAssembly(assembly);
    if (result && out)
        out->m_NativePtr = result;
    return result;
}

HaxAssembly HaxAssembly::Find(const char* assembly)
{
    HaxAssembly result;
    HAX_ASSERT(Exists(assembly, &result), std::format("Assembly {} not found", assembly).c_str());
    return result;
}

HaxClass HaxAssembly::FindClass(const char* nameSpace, const char* klass)
{
    void* nativeClass = HaxSdk::GetGlobals().Backend == HaxBackend_Mono 
        ? (void*)Mono::FindClass((MonoAssembly*)m_NativePtr, nameSpace, klass) 
        : (void*)Il2Cpp::FindClass((Il2CppAssembly*)m_NativePtr, nameSpace, klass);
    HAX_ASSERT(nativeClass, std::format("Class {}.{} not found in {}", nameSpace, klass, GetName()).c_str());
    return HaxClass(nativeClass);
}

const char* HaxAssembly::GetName()
{
    return HaxSdk::GetGlobals().Backend == HaxBackend_Mono
        ? mono_image_get_name(mono_assembly_get_image((MonoAssembly*)m_NativePtr))
        : il2cpp_image_get_name(il2cpp_assembly_get_image((Il2CppAssembly*)m_NativePtr));
}

HaxClass HaxClass::Find(const char* assembly, const char* nameSpace, const char* klass)
{
    return HaxAssembly::Find(assembly).FindClass(nameSpace, klass);
}

const char* HaxClass::GetName()
{
    return HaxSdk::GetGlobals().Backend == HaxBackend_Mono
        ? mono_class_get_name((MonoClass*)m_NativePtr)
        : ((Il2CppClass*)m_NativePtr)->Name;
}

const char* HaxClass::GetNamespace()
{
    return HaxSdk::GetGlobals().Backend == HaxBackend_Mono
        ? mono_class_get_namespace((MonoClass*)m_NativePtr)
        : ((Il2CppClass*)m_NativePtr)->NameSpace;
}

int32_t HaxClass::FindField(const char* field)
{
    int32_t offset = HaxSdk::GetGlobals().Backend == HaxBackend_Mono
        ? Mono::FindField((MonoClass*)m_NativePtr, field)
        : Il2Cpp::FindField((Il2CppClass*)m_NativePtr, field);
    HAX_ASSERT(offset >= 0, std::format("Field {} not found in {}.{}", field, GetNamespace(), GetName()).c_str());
    return offset;
}

void* HaxClass::FindStaticField(const char* field)
{
    void* ptr = HaxSdk::GetGlobals().Backend == HaxBackend_Mono
        ? Mono::FindStaticField((MonoClass*)m_NativePtr, field)
        : Il2Cpp::FindStaticField((Il2CppClass*)m_NativePtr, field);
    HAX_ASSERT(ptr, std::format("Static field {} not found in {}.{}", field, GetNamespace(), GetName()).c_str());
    return ptr;
}

ManagedMethod HaxClass::FindMethod(const char* name, const char* signature)
{
    bool isMono = HaxSdk::GetGlobals().Backend == HaxBackend_Mono;

    void* backendMethod = isMono
        ? (void*)Mono::FindMethod((MonoClass*)m_NativePtr, name, signature)
        : (void*)Il2Cpp::FindMethod((Il2CppClass*)m_NativePtr, name, signature);

    bool hasSignature = signature && signature[0];
    HAX_ASSERT(backendMethod, hasSignature
        ? std::format("Method {} of {} not found in {}.{}", name, signature, GetNamespace(), GetName()).c_str()
        : std::format("Method {} not found in {}.{}", name, GetNamespace(), GetName()).c_str());

    ManagedMethod managedMethod(backendMethod);
    managedMethod.Address = isMono ? mono_compile_method((MonoMethod*)backendMethod) : ((Il2CppMethod*)backendMethod)->Pointer;
    managedMethod.Thunk = (isMono && mono_method_get_unmanaged_thunk) ? mono_method_get_unmanaged_thunk((MonoMethod*)backendMethod) : nullptr;
    return managedMethod;
}

void* ManagedMethod::Invoke(void* __this, void** args)
{
    HAX_ASSERT(HaxSdk::GetGlobals().Backend == HaxBackend_Mono, "Invoke is deprecated in IL2CPP");
    MonoObject* exc = nullptr;
    void* result = (void*)mono_runtime_invoke((MonoMethod*)m_NativePtr, __this, args, &exc);

    if (exc)
        throw std::runtime_error("Invoke failed");
    return result;
}

void ManagedMethod::Hook(void** addr, void* hookFunc)
{
    DetourAttach(addr, hookFunc);
}

HaxClass HaxObject::GetClass()
{
    void* backendClass = HaxSdk::GetGlobals().Backend == HaxBackend_Mono
        ? (void*)mono_object_get_class((MonoObject*)m_NativePtr)
        : (void*)((Il2CppObject*)m_NativePtr)->klass;
    return HaxClass(backendClass);
}

void* HaxObject::Unbox()
{
    HAX_ASSERT(HaxSdk::GetGlobals().Backend == HaxBackend_Mono, "Unbox is deprecated in IL2CPP");
    return mono_object_unbox((MonoObject*)m_NativePtr);
}