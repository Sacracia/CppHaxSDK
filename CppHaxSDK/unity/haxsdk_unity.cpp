#include "haxsdk_unity.h"

static const char* module = "UnityEngine.CoreModule";
static const char* nameSpace = "UnityEngine";

namespace Unity {
    Class* AsyncOperation::GetClass() {
        static Class* pClass = Class::Find(module, nameSpace, "AsyncOperation");
        return pClass;
    }

    bool AsyncOperation::GetIsDone() {
        static HaxMethod<bool(*)(AsyncOperation*)> method(AsyncOperation::GetClass()->FindMethod("get_isDone"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(bool*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    float AsyncOperation::GetProgress() {
        static HaxMethod<float(*)(AsyncOperation*)> method(AsyncOperation::GetClass()->FindMethod("get_progress"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(float*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    Class* Behaviour::GetClass() {
        static Class* pClass = Class::Find(module, nameSpace, "Behaviour");
        return pClass;
    }

    bool Behaviour::GetEnabled() {
        static HaxMethod<bool(*)(Behaviour*)> method(Behaviour::GetClass()->FindMethod("get_enabled"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(bool*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    bool Behaviour::GetIsActiveAndEnabled() {
        static HaxMethod<bool(*)(Behaviour*)> method(Behaviour::GetClass()->FindMethod("get_isActiveAndEnabled"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(bool*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    void Behaviour::SetEnabled(bool value) {
        static HaxMethod<void(__fastcall*)(Behaviour*,bool)> method(Behaviour::GetClass()->FindMethod("set_enabled"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    Class* BoxCollider::GetClass() {
        static Class* pClass = Class::Find(module, nameSpace, "BoxCollider");
        return pClass;
    }

    Vector3 BoxCollider::InternalGetCenter() {
        static HaxMethod<void(*)(BoxCollider*,Vector3*)> method(BoxCollider::GetClass()->FindMethod("INTERNAL_get_center"));
        Vector3 result;
        method.ptr(this, &result);
        return result;
    }

    Vector3 BoxCollider::InternalGetSize() {
        static HaxMethod<void(*)(BoxCollider*,Vector3*)> method(BoxCollider::GetClass()->FindMethod("INTERNAL_get_size"));
        Vector3 result;
        method.ptr(this, &result);
        return result;
    }

    void BoxCollider::InternalSetCenter(Vector3 value) {
        static HaxMethod<void(*)(BoxCollider*,Vector3*)> method(BoxCollider::GetClass()->FindMethod("INTERNAL_set_center"));
        method.ptr(this, &value);
    }

    void BoxCollider::InternalSetSize(Vector3 value) {
        static HaxMethod<void(*)(BoxCollider*,Vector3*)> method(BoxCollider::GetClass()->FindMethod("INTERNAL_set_size"));
        method.ptr(this, &value);
    }

    Vector3 BoxCollider::GetCenter() {
        static HaxMethod<Vector3(*)(BoxCollider*)> method(BoxCollider::GetClass()->FindMethod("get_center"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(Vector3*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    Vector3 BoxCollider::GetSize() {
        static HaxMethod<Vector3(*)(BoxCollider*)> method(BoxCollider::GetClass()->FindMethod("get_size"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(Vector3*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    /*struct Camera {
        Camera() = delete;
        Camera(const Camera&) = delete;
    public:
        static ::Class* GetClass();
    public:
        static Camera* GetMain();
        Vector3 WorldToScreenPoint(Vector3 position);
        Vector3 WorldToScreenPoint_Injected(Vector3 position);
    };*/

    Class* Camera::GetClass() {
        static Class* pClass = Class::Find(module, nameSpace, "Camera");
        return pClass;
    }

    Camera* Camera::GetMain() {
        static HaxMethod<Camera*(*)()> method(Camera::GetClass()->FindMethod("get_main"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? (Camera*)method.Invoke(nullptr, nullptr) : method.ptr();
    }

    Vector3 Camera::WorldToScreenPoint(Vector3 position) {
        static HaxMethod<Vector3(__fastcall*)(Camera*,Vector3)> method(Camera::GetClass()->FindMethod("WorldToScreenPoint", "UnityEngine.Vector3(UnityEngine.Vector3)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &position };
            return *(Vector3*)method.Invoke(this, args)->Unbox();
        }

        return method.ptr(this, position);
    }

    Vector3 Camera::WorldToScreenPoint_Injected(Vector3 position, Camera::MonoOrStereoscopicEye eye) {
        static HaxMethod<void(__fastcall*)(Vector3*,Int32,Vector3*)> method(Camera::GetClass()->FindMethod("WorldToScreenPoint_Injected"));
        Vector3 res;
        method.ptr(&position, eye, &res);
        return res;
    }

    Vector3 Camera::INTERNAL_CALL_WorldToScreenPoint(Vector3 position) {
        static HaxMethod<void(__fastcall*)(Camera*,Vector3*,Vector3*)> method(Camera::GetClass()->FindMethod("INTERNAL_CALL_WorldToScreenPoint"));
        Vector3 res;
        method.ptr(this, &position, &res);
        return res;
    }

    Class* Collider::GetClass() {
        static Class* pClass = Class::Find(module, nameSpace, "Collider");
        return pClass;
    }

    bool Collider::GetEnabled() {
        static HaxMethod<bool(__fastcall*)(Collider*)> method(Collider::GetClass()->FindMethod("get_enabled"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(bool*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    void Collider::SetEnabled(bool value) {
        static HaxMethod<void(__fastcall*)(Collider*,bool)> method(Collider::GetClass()->FindMethod("set_enabled"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    Class* Object::GetClass() {
        static Class* pClass = Class::Find(module, nameSpace, "Object");
        return pClass;
    }

    System::Array<Object*>* Object::FindObjectsOfType(System::Type* pType) {
        static HaxMethod<System::Array<Object*>*(*)(System::Type*)> method(Object::GetClass()->FindMethod("FindObjectsOfType", "UnityEngine.Object[](System.Type)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pType };
            return (System::Array<Object*>*)method.Invoke(nullptr, args);
        }
        return method.ptr(pType);
    }

    Object* Object::FindObjectOfType(System::Type* pType) {
        static HaxMethod<Object*(*)(System::Type*)> method(Object::GetClass()->FindMethod("FindObjectOfType", "UnityEngine.Object(System.Type)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pType };
            return (Object*)method.Invoke(nullptr, args);
        }

        return method.ptr(pType);
    }

    void Object::Destroy(Object* pObj, float t) {
        static HaxMethod<void(*)(Object*,float)> method(Object::GetClass()->FindMethod("Destroy"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pObj, &t};
            method.Invoke(nullptr, args);
            return;
        }

        method.ptr(pObj, t);
    }

    System::String* Object::GetName() {
        static HaxMethod<System::String*(*)(Object*)> method(Object::GetClass()->FindMethod("get_name"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? (System::String*)method.Invoke(this, nullptr) : method.ptr(this);
    }

    Class* GameObject::GetClass() {
        static Class* pClass = Class::Find(module, nameSpace, "GameObject");
        return pClass;
    }

    GameObject* GameObject::New(const char* name) {
        GameObject* newGameObject = (GameObject*)System::Object::New(GameObject::GetClass());
        auto* pName = System::String::New(name);

        static HaxMethod<void(*)(GameObject*, System::String*)> method(GameObject::GetClass()->FindMethod(".ctor", "System.Void(System.String)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[1] = { pName };
            method.Invoke(newGameObject, args);
            return newGameObject;
        }
        
        method.ptr(newGameObject, pName);
        return newGameObject;
    }

    Transform* GameObject::GetTransform() {
        static HaxMethod<Transform*(*)(GameObject*)> method(GameObject::GetClass()->FindMethod("get_transform"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? (Transform*)method.Invoke(this, nullptr) : method.ptr(this);
    }

    void GameObject::SetLayer(Int32 value) {
        static HaxMethod<void(*)(GameObject*,Int32)> method(GameObject::GetClass()->FindMethod("set_layer"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    bool GameObject::GetActiveSelf() {
        static HaxMethod<bool(*)(GameObject*)> method(GameObject::GetClass()->FindMethod("get_activeSelf"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(bool*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    Component* GameObject::GetComponent(System::Type* pType) {
        static HaxMethod<Component*(*)(GameObject*,System::Type*)> method(GameObject::GetClass()->FindMethod("GetComponent", "UnityEngine.Component(System.Type)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pType };
            return (Component*)method.Invoke(this, args);
        }

        return method.ptr(this, pType);
    }

    Component* GameObject::AddComponent(System::Type* pType) {
        static HaxMethod<Component*(*)(GameObject*,System::Type*)> method(GameObject::GetClass()->FindMethod("AddComponent", "UnityEngine.Component(System.Type)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pType };
            return (Component*)method.Invoke(this, args);
        }

        return method.ptr(this, pType);
    }

    System::Array<Component*>* GameObject::GetComponentsInChildren(System::Type* pType, bool includeInactive = false) {
        static HaxMethod<System::Array<Component*>*(*)(GameObject*,System::Type*,bool)> method(GameObject::GetClass()->FindMethod("GetComponentsInChildren", "Component[](System.Type,System.Boolean)"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { pType, &includeInactive };
            return (System::Array<Component*>*)method.Invoke(this, args);
        }

        return method.ptr(this, pType, includeInactive);
    }

    void GameObject::SetActive(bool value) {
        static HaxMethod<void(*)(GameObject*,bool)> method(GameObject::GetClass()->FindMethod("set_active"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return; 
        }

        return method.ptr(this, value);
    }

    bool GameObject::GetActive() {
        static HaxMethod<bool(*)(GameObject*)> method(GameObject::GetClass()->FindMethod("get_active"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(bool*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    Transform* Component::GetTransform() {
        static HaxMethod<Transform*(*)(Component*)> method(Component::GetClass()->FindMethod("get_transform"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? (Transform*)method.Invoke(this, nullptr) : method.ptr(this);
    }

    Class* Component::GetClass() {
        static Class* pClass = Class::Find(module, nameSpace, "Component");
        return pClass;
    }

    GameObject* Component::GetGameObject() {
        static HaxMethod<GameObject*(*)(Component*)> method(Component::GetClass()->FindMethod("get_gameObject"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? (GameObject*)method.Invoke(this, nullptr) : method.ptr(this);
    }

    System::Type* KeyCode::GetSystemType() {
        static System::Type* pType = Class::Find(module, nameSpace, "KeyCode")->GetSystemType();
        return pType;
    }

    Class* Transform::GetClass() {
        static Class* pClass = Class::Find(module, nameSpace, "Transform");
        return pClass;
    }

    Vector3 Transform::GetPosition() {
        static HaxMethod<Vector3(*)(Transform*)> method(Transform::GetClass()->FindMethod("get_position"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(Vector3*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    void Transform::SetPosition(Vector3 value) {
        static HaxMethod<void(*)(Transform*,Vector3)> method(Transform::GetClass()->FindMethod("set_position"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    Transform* Transform::GetParent() {
        static HaxMethod<Transform*(*)(Transform*)> method(Transform::GetClass()->FindMethod("get_parent"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? (Transform*)method.Invoke(this, nullptr) : method.ptr(this);
    }

    void Transform::SetParent(Transform* value) {
        static HaxMethod<void(*)(Transform*,Transform*)> method(Transform::GetClass()->FindMethod("set_parent"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    Vector3 Transform::GetForward() {
        static HaxMethod<Vector3(*)(Transform*)> method(Transform::GetClass()->FindMethod("get_forward"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(Vector3*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    Class* Light::GetClass() {
        static Class* pClass = Class::Find(module, nameSpace, "Light");
        return pClass;
    }

    void Light::SetIntensity(float value) {
        static HaxMethod<void(*)(Light*,float)> method(Light::GetClass()->FindMethod("set_intensity"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    float Light::GetIntensity() {
        static HaxMethod<float(*)(Light*)> method(Light::GetClass()->FindMethod("get_intensity"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(float*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }

    void Light::SetRange(float value) {
        static HaxMethod<void(*)(Light*, float)> method(Light::GetClass()->FindMethod("set_range"));
        if (HaxSdk::GetGlobals().backend & HaxBackend_Mono) {
            void* args[] = { &value };
            method.Invoke(this, args);
            return;
        }

        method.ptr(this, value);
    }

    float Light::GetRange() {
        static HaxMethod<float(*)(Light*)> method(Light::GetClass()->FindMethod("get_range"));
        return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? *(float*)method.Invoke(this, nullptr)->Unbox() : method.ptr(this);
    }
}