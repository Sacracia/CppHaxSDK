
#ifdef MONO_API_FUNC
MONO_API_FUNC(MonoDomain*, mono_get_root_domain, ());
MONO_API_FUNC(MonoAssembly*, mono_domain_assembly_open, (MonoDomain* domain, const char* name));
MONO_API_FUNC(MonoThread*, mono_thread_attach, (MonoDomain* domain));
MONO_API_FUNC(MonoDomain*, mono_domain_get, ());

// image
MONO_API_FUNC(MonoImage*, mono_assembly_get_image, (MonoAssembly* assembly));

// class
MONO_API_FUNC(MonoClass*, mono_class_from_name, (MonoImage* image, const char* name_space, const char* name));
MONO_API_FUNC(MonoMethod*, mono_class_get_methods, (MonoClass* klass, void** iter));
MONO_API_FUNC(MonoClassField*, mono_class_get_field_from_name, (MonoClass* klass, const char* name));
MONO_API_FUNC(MonoVTable*, mono_class_vtable, (MonoDomain* domain, MonoClass* klass));

// method
MONO_API_FUNC(const char*, mono_method_full_name, (MonoMethod* method, int32_t signature));
MONO_API_FUNC(void*, mono_compile_method, (MonoMethod* method));

// field
MONO_API_FUNC(void*, mono_vtable_get_static_field_data, (MonoVTable* vt));

#endif
