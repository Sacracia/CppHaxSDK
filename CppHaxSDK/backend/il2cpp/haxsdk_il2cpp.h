#pragma once

#include <cstdint>

#undef GetMessage

/*
 * Field Attributes (21.1.5).
 */

#define FIELD_ATTRIBUTE_FIELD_ACCESS_MASK     0x0007
#define FIELD_ATTRIBUTE_COMPILER_CONTROLLED   0x0000
#define FIELD_ATTRIBUTE_PRIVATE               0x0001
#define FIELD_ATTRIBUTE_FAM_AND_ASSEM         0x0002
#define FIELD_ATTRIBUTE_ASSEMBLY              0x0003
#define FIELD_ATTRIBUTE_FAMILY                0x0004
#define FIELD_ATTRIBUTE_FAM_OR_ASSEM          0x0005
#define FIELD_ATTRIBUTE_PUBLIC                0x0006

#define FIELD_ATTRIBUTE_STATIC                0x0010
#define FIELD_ATTRIBUTE_INIT_ONLY             0x0020
#define FIELD_ATTRIBUTE_LITERAL               0x0040
#define FIELD_ATTRIBUTE_NOT_SERIALIZED        0x0080
#define FIELD_ATTRIBUTE_SPECIAL_NAME          0x0200
#define FIELD_ATTRIBUTE_PINVOKE_IMPL          0x2000

 /* For runtime use only */
#define FIELD_ATTRIBUTE_RESERVED_MASK         0x9500
#define FIELD_ATTRIBUTE_RT_SPECIAL_NAME       0x0400
#define FIELD_ATTRIBUTE_HAS_FIELD_MARSHAL     0x1000
#define FIELD_ATTRIBUTE_HAS_DEFAULT           0x8000
#define FIELD_ATTRIBUTE_HAS_FIELD_RVA         0x0100

struct Il2CppImage;
struct Il2CppDomain;
struct Il2CppThread;
struct Il2CppString;
struct Il2CppException;
struct MonitorData;
struct Il2CppType;
struct Il2CppClass;
struct Il2CppAssembly;
struct Il2CppObject;
struct Il2CppAppDomain;
struct Il2CppMarshalByRefObject;
struct Il2CppAppDomainSetup;
struct Il2CppAppContext;
struct Il2CppAssemblyName;
struct Il2CppReflectionType;
struct Il2CppField;                 // renamed from FieldInfo
struct Il2CppMethod;                // renamed from MethodInfo
struct Il2CppArrayBounds;
struct Il2CppArrayType;
struct Il2CppTypeDefinitionIndex;
struct Il2CppGenericClass;
struct Il2CppExceptionWrapper;
struct Il2CppArray;

using Il2CppVTable = Il2CppClass;

namespace il2cpp
{
    void Initialize();
}

struct Il2CppObject
{
    inline Il2CppClass* GetClass() { return klass; }

    static Il2CppObject* Box(Il2CppClass* klass, void* data);
    static Il2CppObject* New(Il2CppClass* klass);

    void Ctor();

    union
    {
        Il2CppClass* klass;
        Il2CppVTable* vtable;
    };
    MonitorData* monitor;
};

struct Il2CppAssembly
{
    Il2CppImage* GetImage();
};

struct Il2CppImage
{
    static Il2CppImage* GetCorlib();
    static Il2CppImage* GetUnityCore();
    Il2CppClass* GetClass(const char* nameSpace, const char* name);
};

struct Il2CppDomain
{
    static Il2CppDomain* GetRoot();
    Il2CppAssembly* GetAssembly(const char* name);
};

struct Il2CppException
{
    inline Il2CppString*& GetMessage() { return message; }
    inline Il2CppString* GetStackTrace() { return stack_trace; }

#if !defined(NET_4_0)
    /* Stores the IPs and the generic sharing infos
       (vtable/MRGCTX) of the frames. */
    Il2CppArray* trace_ips;
    Il2CppException* inner_ex;
    Il2CppString* message;
    Il2CppString* help_link;
    Il2CppString* class_name;
    Il2CppString* stack_trace;
    Il2CppString* remote_stack_trace;
    int32_t    remote_stack_index;
    int32_t hresult;
    Il2CppString* source;
    Il2CppObject* _data;
#else
    Il2CppString* className;
    Il2CppString* message;
    Il2CppObject* _data;
    Il2CppException* inner_ex;
    Il2CppString* _helpURL;
    Il2CppArray* trace_ips;
    Il2CppString* stack_trace;
    Il2CppString* remote_stack_trace;
    int remote_stack_index;
    Il2CppObject* _dynamicMethods;
    int32_t hresult;
    Il2CppString* source;
    Il2CppObject* safeSerializationManager;
    Il2CppArray* captured_traces;
    Il2CppArray* native_trace_ips;
#endif
};

struct Il2CppThread
{
    static Il2CppThread* Attach();
    void Detach();
};

struct Il2CppClass
{
    Il2CppField* GetField(const char* name);
    Il2CppMethod* GetMethod(const char* name, const char* signature);
    void* GetStaticFieldData();
    Il2CppType* GetType();
};

struct Il2CppMethod
{
    Il2CppObject* Invoke(void* __this, void** args, Il2CppException** ex);
    inline void* GetPointer() { return m_Pointer; }

    void* m_Pointer;
};

struct Il2CppType
{
    Il2CppReflectionType* GetReflectionType();
    Il2CppClass* GetClass();

    inline bool IsLiteral() { return attrs & FIELD_ATTRIBUTE_LITERAL; }

    union
    {
        int32_t klassIndex; /* for VALUETYPE and CLASS */
        const Il2CppType* type;   /* for PTR and SZARRAY */
        Il2CppArrayType* array; /* for ARRAY */
        //MonoMethodSignature *method;
        int32_t genericParameterIndex; /* for VAR and MVAR */
        Il2CppGenericClass* generic_class; /* for GENERICINST */
    };
    unsigned int attrs : 16; /* param attributes or field flags */

    /* ... */
};

struct Il2CppField
{
    inline bool IsStatic() { return type->attrs & FIELD_ATTRIBUTE_STATIC; }
    inline int GetOffset() { return offset; }
    inline const char* GetName() { return name; }
    inline bool IsLiteral() { return type->attrs & FIELD_ATTRIBUTE_LITERAL; }

    void GetStaticValue(void* value);
    void* GetValuePtr(void* __this);

private:
    const char* name;
    const Il2CppType* type;
    Il2CppClass* parent;
    int offset; // If offset is -1, then it's thread static
    uint32_t token;
};

struct Il2CppReflectionType : Il2CppObject
{
    inline Il2CppType* GetType() { return type; }

    Il2CppType* type;
};

struct Il2CppExceptionWrapper
{
    Il2CppException* ex;
};

struct Il2CppString
{
    static Il2CppString* New(const char* str);
};

struct Il2CppGCHandle
{
    static uint32_t New(Il2CppObject* obj, bool pinned);
    static uint32_t NewWeak(Il2CppObject* obj, bool trackResurrection);
    static Il2CppObject* GetTarget(uint32_t handle);
    static void Free(uint32_t handle);
};