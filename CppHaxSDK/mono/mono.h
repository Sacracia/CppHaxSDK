#pragma once

#include "mono_api_classes.h"

namespace mono {
	void    Initialize();
    void*   GetStaticField(MonoClass*& klass, const char* assemblyName, const char* name_space,
                           const char* klassName, const char* fieldName);
    int     GetFieldOffset(MonoClass*& klass, MonoObject* object, const char* fieldName);
}