
#ifdef DO_API
DO_API(MonoDomain*, mono_get_root_domain, ());
DO_API(MonoThread*, mono_thread_attach, (MonoDomain* domain));
DO_API(MonoDomain*, mono_domain_get, ());
#endif
