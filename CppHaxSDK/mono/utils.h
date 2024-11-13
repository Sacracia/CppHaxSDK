#pragma once

#include "mono_game_classes.h"
#define MONO_API_FUNC(r, n, p) extern r(*n)p
#include "mono_api_functions.h"
#undef MONO_API_FUNC

#ifndef HAX_ASSERT
#include <assert.h>
#define HAX_ASSERT(_EXPR) assert(_EXPR)
#endif

#ifndef FIELD
#define FIELD(c, t, n, a, ns)                                                                           \
t* c::n() {                                                                                             \
    static int offset = -1;                                                                             \
    if (offset < 0) {                                                                                   \
        if (klass == nullptr) {                                                                         \
            klass = utils::GetMonoClass(a, ns, #c);                                                     \
        }                                                                                               \
        offset = mono_class_get_field_from_name(klass, #n)->offset;                                     \
    }                                                                                                   \
    return (t*)((char*)this + offset);                                                                  \
}
#endif

#ifndef STATIC_FIELD
#define STATIC_FIELD(c, t, n, a, ns)\
t* c::n() {                                                                                             \
    static t* address = nullptr;                                                                        \
    if (address == nullptr) {                                                                           \
        if (klass == nullptr) {                                                                         \
            klass = utils::GetMonoClass(a, ns, #c);                                                     \
        }                                                                                               \
        MonoVTable* vtable = mono_class_vtable(domain, klass);                                          \
        MonoClassField* field = mono_class_get_field_from_name(klass, #n);                              \
        void* ptr = mono_vtable_get_static_field_data(vtable);                                          \
        address = (t*)((char*)mono_vtable_get_static_field_data(vtable) + field->offset);               \
    }                                                                                                   \
    return address;                                                                                     \
}
#endif

extern MonoDomain* domain;

namespace utils {
    MonoClass* GetMonoClass(const char* assemblyName, const char* name_space, const char* klassName);
}