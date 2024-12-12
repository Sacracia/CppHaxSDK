#pragma once

#ifdef HAX_MONO
#include "../backends/haxsdk_mono.h"
#elif defined(HAX_IL2CPP)
#include "../backends/haxsdk_il2cpp.h"
#endif

namespace HaxSdk {
    void InitializeUnityData();
}

template <typename T>
struct Array {
    uint64_t              size() { return max_length; }
    T*                    data() { return vector; }
private:
    void*                 __space[2];
    uint64_t              max_length;
    T                     vector[32];
};

template <class T>
struct List {
    void*               __space[2];
    Array<T>*           items;
    int32_t             size;
    int32_t             version;
    void*               syncRoot;
};

struct Vector3 {
    static float        Distance(Vector3 a, Vector3 b);
public:
    float x;
    float y;
    float z;
};

struct Object {
    static Array<BackendObject*>*   FindObjectsOfType(SystemType* type);
    static BackendObject*           FindObjectOfType(SystemType* type);
};

struct Transform {
    Vector3             get_position();
    void                set_position(Vector3 value);
};
struct Component : BackendObject {
    Transform*          get_transform();
};

struct Camera {
    static Camera*      main();
    Vector3             WorldToScreenPoint(Vector3 position);
};

struct Screen {
    static int          width();
    static int          height();
};