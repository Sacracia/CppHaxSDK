#pragma once

#undef GetMessage

#include <cstdint>

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

struct MonoAssembly;
struct MonoAssemblyName;
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
struct MonoThreadsSync;
struct MonoAppDomainSetup;
struct MonoAppContext;
struct MonoGHashTable;
struct MonoAppDomain;
struct MonoReflectionType;
struct MonoArrayType;
struct MonoGenericParam;
struct MonoGenericClass;
struct MonoExceptionWrapper;
struct MonoArrayBounds;
struct MonoArray;
struct MonoField;

namespace mono
{
    void Initialize();
}

enum MonoTypeEnum : uint8_t {
	MONO_TYPE_END        = 0x00,       /* End of List */
	MONO_TYPE_VOID       = 0x01,
	MONO_TYPE_BOOLEAN    = 0x02,
	MONO_TYPE_CHAR       = 0x03,
	MONO_TYPE_I1         = 0x04,
	MONO_TYPE_U1         = 0x05,
	MONO_TYPE_I2         = 0x06,
	MONO_TYPE_U2         = 0x07,
	MONO_TYPE_I4         = 0x08,
	MONO_TYPE_U4         = 0x09,
	MONO_TYPE_I8         = 0x0a,
	MONO_TYPE_U8         = 0x0b,
	MONO_TYPE_R4         = 0x0c,
	MONO_TYPE_R8         = 0x0d,
	MONO_TYPE_STRING     = 0x0e,
	MONO_TYPE_PTR        = 0x0f,       /* arg: <type> token */
	MONO_TYPE_BYREF      = 0x10,       /* arg: <type> token */
	MONO_TYPE_VALUETYPE  = 0x11,       /* arg: <type> token */
	MONO_TYPE_CLASS      = 0x12,       /* arg: <type> token */
	MONO_TYPE_VAR	     = 0x13,	   /* number */
	MONO_TYPE_ARRAY      = 0x14,       /* type, rank, boundsCount, bound1, loCount, lo1 */
	MONO_TYPE_GENERICINST= 0x15,	   /* <type> <type-arg-count> <type-1> \x{2026} <type-n> */
	MONO_TYPE_TYPEDBYREF = 0x16,
	MONO_TYPE_I          = 0x18,
	MONO_TYPE_U          = 0x19,
	MONO_TYPE_FNPTR      = 0x1b,	      /* arg: full method signature */
	MONO_TYPE_OBJECT     = 0x1c,
	MONO_TYPE_SZARRAY    = 0x1d,       /* 0-based one-dim-array */
	MONO_TYPE_MVAR	     = 0x1e,       /* number */
	MONO_TYPE_CMOD_REQD  = 0x1f,       /* arg: typedef or typeref token */
	MONO_TYPE_CMOD_OPT   = 0x20,       /* optional arg: typedef or typref token */
	MONO_TYPE_INTERNAL   = 0x21,       /* CLR internal type */

	MONO_TYPE_MODIFIER   = 0x40,       /* Or with the following types */
	MONO_TYPE_SENTINEL   = 0x41,       /* Sentinel for varargs method signature */
	MONO_TYPE_PINNED     = 0x45,       /* Local var that points to pinned object */

	MONO_TYPE_ENUM       = 0x55        /* an enumeration */
};

struct MonoVTable
{
    MonoClass* klass;

    /* ... */
};

struct MonoObject
{
    static MonoObject* Box(MonoClass* klass, void* data);
    static MonoObject* New(MonoClass* klass);

    void Ctor();
    inline MonoClass* GetClass() { return vtable->klass; }

    MonoVTable* vtable;
    MonoThreadsSync* synchronisation;
};

struct MonoException : MonoObject
{
    inline MonoString*& GetMessage() { return message; }
    inline MonoString* GetStackTrace() { return stack_trace; }

    static MonoException* GetNullReference();
    static MonoException* GetArgumentOutOfRange();

    MonoString* class_name;
    MonoString* message;
    MonoObject* _data;
    MonoObject* inner_ex;
    MonoString* help_link;
    /* Stores the IPs and the generic sharing infos
       (vtable/MRGCTX) of the frames. */
    MonoArray** trace_ips;
    MonoString* stack_trace;
    MonoString* remote_stack_trace;
    int32_t	    remote_stack_index;
    /* Dynamic methods referenced by the stack trace */
    MonoObject* dynamic_methods;
    int32_t	    hresult;
    MonoString* source;
    MonoObject* serialization_manager;
    MonoObject* captured_traces;
    MonoArray** native_trace_ips;
    int32_t caught_in_unmanaged;
};

struct MonoDomain
{
    static MonoDomain* GetRoot();
    MonoAssembly* GetAssembly(const char* name);
};

struct MonoThread
{
    static MonoThread* Attach();
    void Detach();
};

struct MonoAssembly
{
    MonoImage* GetImage();
};

struct MonoImage
{
    static MonoImage* GetCorlib();
    static MonoImage* GetUnityCore();
    MonoClass* GetClass(const char* nameSpace, const char* name);
};

struct MonoClass
{
    MonoVTable* GetVTable();
    MonoField* GetField(const char* name);
    MonoMethod* GetMethod(const char* name, const char* signature);
    void* GetStaticFieldData();
    MonoType* GetType();
};

struct MonoMethod
{
    void* GetPointer();
    void* GetThunk();
    MonoObject* Invoke(void* __this, void** args, MonoException** ex);
};

struct MonoType
{
    MonoReflectionType* GetReflectionType();
    MonoClass* GetClass();

    union {
        MonoClass* klass; /* for VALUETYPE and CLASS */
        MonoType* type;   /* for PTR */
        MonoArrayType* array; /* for ARRAY */
        MonoMethodSignature* method;
        MonoGenericParam* generic_param; /* for VAR and MVAR */
        MonoGenericClass* generic_class; /* for GENERICINST */
    } data;
    uint16_t attrs; /* param attributes or field flags */
    MonoTypeEnum type;

    /* ... */
};

struct MonoField
{
    inline MonoClass* GetParent() { return parent; }
    inline int GetOffset() { return offset; }
    inline bool IsStatic() { return type->attrs & FIELD_ATTRIBUTE_STATIC; }
    inline const char* GetName() { return name; }
    inline bool IsLiteral() { return type->attrs & FIELD_ATTRIBUTE_LITERAL; }

    void GetStaticValue(void* value);

    void* GetValuePtr(void* __this);

    MonoType* type;
    const char* name;
    MonoClass* parent;
    int offset;
};

struct MonoReflectionType : MonoObject
{
    MonoType* type;
};

struct MonoString
{
    static MonoString* New(const char* str);
};

struct MonoGCHandle
{
    static uint32_t New(MonoObject* obj, bool pinned);
    static uint32_t NewWeak(MonoObject* obj, bool trackResurrection);
    static MonoObject* GetTarget(uint32_t handle);
    static void Free(uint32_t handle);
};