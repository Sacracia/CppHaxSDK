#pragma once

#include <cstdint>

#include "haxsdk_backend.h"

using Int32 = int32_t;

namespace System
{
    struct Type;
    struct String;
    struct Object;
}


struct System::String
{

};

struct System::Object
{
    operator bool() const { return m_NativePtr != nullptr; }

    Int32           GetHashCode();
    System::String  ToString();

private:
    void* m_NativePtr;
};
