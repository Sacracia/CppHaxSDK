#ifndef MONO_STRUCTS
#define MONO_STRUCTS
struct MonoThread;
struct MonoDomain;
struct MonoAssembly;
struct MonoObject;
struct MonoType;
struct MonoClass;
struct MonoImage;
struct MonoMethod;
struct MonoString;
struct MonoField;
struct MonoVTable;
struct MonoArray;
#endif

#ifdef DO_API
DO_API(MonoDomain*, mono_get_root_domain, ());
DO_API(MonoThread*, mono_thread_attach, (MonoDomain* domain));
DO_API(MonoDomain*, mono_domain_get, ());
#endif
