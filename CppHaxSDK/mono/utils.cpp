#include "utils.h"

MonoClass* utils::GetMonoClass(const char* assemblyName, const char* name_space, const char* klassName) {
    MonoAssembly* assembly = mono_domain_assembly_open(domain, assemblyName);
    HAX_ASSERT(assembly != nullptr && "mono_domain_assembly_open failed");
    MonoImage* image = mono_assembly_get_image(assembly);
    HAX_ASSERT(image != nullptr && "mono_assembly_get_image failed");
    MonoClass* klass = mono_class_from_name(image, name_space, klassName);
    HAX_ASSERT(klass != nullptr && "mono_class_from_name failed");
    return klass;
}