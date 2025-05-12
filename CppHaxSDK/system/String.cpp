#include "haxsdk_system.h"

#include "../haxsdk_tools.h"

namespace System
{
    Char& String::operator[](int i)
    {
        CHECK_NULL();

        if (i < 0 || i >= GetPointer()->GetLength() - 1)
            throw ArgumentOutOfRangeException::New();

        return GetPointer()->operator[](i);
    }

    const Char& String::operator[](int i) const
    {
        CHECK_NULL();

        if (i < 0 || i >= GetPointer()->GetLength() - 1)
            throw ArgumentOutOfRangeException::New();

        return GetPointer()->operator[](i);
    }

    Char* String::begin()
    { 
        CHECK_NULL();

        return GetPointer()->begin(); 
    }

    const Char* String::begin() const
    {
        CHECK_NULL();

        return GetPointer()->begin();
    }

    Char* String::end()
    {
        CHECK_NULL();

        unsafe::String* native = GetPointer();
        return native->begin() + native->GetLength();
    }

    const Char* String::end() const
    {
        CHECK_NULL();

        unsafe::String* native = GetPointer();
        return native->begin() + native->GetLength();
    }

    Int32 String::GetLength() const
    {
        CHECK_NULL();

        return GetPointer()->GetLength();
    }

    Char* String::GetRawStringData() const
    {
        CHECK_NULL();

        return GetPointer()->GetRawStringData();
    }

    const char* String::UTF8() const
    {
        CHECK_NULL();

        unsafe::String* native = GetPointer();
        return HaxSdk::UTF8(native->GetRawStringData(), native->GetLength());
    }
}