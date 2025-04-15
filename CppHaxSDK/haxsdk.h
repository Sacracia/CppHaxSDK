
#include <cstdint>

using Int32 = int32_t;

namespace System
{
    struct Object;
    struct Array;
    struct Type;
    struct String;
}

struct System::Object
{
    Object(const Object&) = default;
    Object(Object&&) noexcept = default;
    Object& operator=(const Object&) = default;
    Object& operator=(Object&&) noexcept = default;

    operator bool() const { return m_NativePtr != nullptr; }
    operator void* () const { return m_NativePtr; }

    Int32 GetHashCode();
    Type GetType();
    String ToString();

protected:
    void* m_NativePtr; // MonoObject* or Il2CppObject*
};