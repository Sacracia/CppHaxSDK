#pragma once

#include <string>

namespace mono {
	void        Initialize();
    void*       GetFuncAddress(const char* assemblyName, const char* name_space, 
                           const char* klassName, const char* methodSig);
    int         GetFieldOffset(const char* assemblyName, const char* name_space, 
                           const char* klassName, const char* fieldName);
    void*       GetStaticFieldAddress(const char* assemblyName, const char* name_space, 
                                  const char* klassName, const char* fieldName);
}