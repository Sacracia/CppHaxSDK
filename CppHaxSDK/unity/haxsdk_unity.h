#pragma once

#include "../haxsdk.h"

namespace HaxSdk {
    void InitializeUnity();
}

namespace Unity {
    struct Transform;
}

namespace Unity {
    struct Object : System::Object {

    };

    struct Component : Unity::Object {
        Transform* GetTransform();
        GameObject* GetGameObject();
    };

    struct Transform : Component {

    };

    struct GameObject : Unity::Object {
        static inline Class*                    GetClass()                                  { return Class::Find(module, nameSpace, "GameObject"); }
        static inline GameObject*               New()                                       { return (GameObject*)System::Object::New(GetClass())->Ctor(); }
        static GameObject*                      New(const char* name);
        /*void                                    SetLayer(int32_t value);
        bool                                    GetActiveSelf();
        Component*                              GetComponent(System::Type* type);
        Component*                              AddComponent(System::Type* componentType);
        System::Array<Component*>*              GetComponentsInChildren(System::Type* type);
        void                                    SetActive(bool value);
        bool                                    GetActive();*/
    };
}