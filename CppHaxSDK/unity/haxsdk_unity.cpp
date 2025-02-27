#include "haxsdk_unity.h"

static const char* module = "UnityEngine.CoreModule";
static const char* nameSpace = "UnityEngine";

namespace Unity {
    GameObject* GameObject::New(const char* name) {
        GameObject* newGameObject = (GameObject*)System::Object::New(GameObject::GetClass());
        auto* pName = System::String::New(name);

        static HaxMethod<void(*)(GameObject*, System::String*)> method(GameObject::GetClass()->FindMethod(".ctor", "System.Void(System.String)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_IL2CPP) {
            method.ptr(newGameObject, pName);
        }
        else {
            void* args[1] = { pName };
            method.Invoke(newGameObject, args);
        }

        return newGameObject;
    }

    Transform* Component::GetTransform() {
        static HaxMethod<Transform*(*)(Component*)> method(GameObject::GetClass()->FindMethod("get_transform"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? (Transform*)method.Invoke(this, nullptr) : method.ptr(this);
    }


}