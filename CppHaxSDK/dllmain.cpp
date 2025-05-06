#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <format>

#include "haxsdk_logger.h"
#include "haxsdk_gui.h"
#include "haxsdk_system.h"
#include "haxsdk_tools.h"

using namespace System;

static FieldInfo EnemyDirector__enemiesSpawned = FieldInfo("Assembly-CSharp", "", "EnemyDirector", "enemiesSpawned");
static FieldInfo EnemyDirector__instance = FieldInfo("Assembly-CSharp", "", "EnemyDirector", "instance");
static FieldInfo EnemyParent__enemyName = FieldInfo("Assembly-CSharp", "", "EnemyParent", "enemyName");

static backend::Class* v3Class;

struct EnemyParent : Object
{
    String enemyName() const { CHECK_NULL(m_ManagedPtr); return EnemyParent__enemyName.GetValue<String>(m_Object); }
};

struct EnemyDirector : Object
{
    static EnemyDirector instance() 
    { 
        return EnemyDirector__instance.GetValue<EnemyDirector>(null);
    }

    List<EnemyParent> enemiesSpawned()
    {
        if (!m_ManagedPtr)
            throw NullReferenceException::New();

        return EnemyDirector__enemiesSpawned.GetValue<List<EnemyParent>>(m_Object);
    }
};

struct Vector3
{
    float x, y, z;
};

static GCHandle strongHandle = GCHandle(0);
static GCHandle weakHandle = GCHandle(0);

void RenderMenu()
{
    if (HaxWindow window("Window"); window)
    {
        if (ImGui::Button("Setup handles"))
        {
            Vector3 v(1.f, 2.f, 3.f);
            System::Object weak(backend::Object::Box(v3Class, &v));
            weakHandle = std::move(GCHandle::New(weak, System::Weak));
            printf("Weak obj ptr %p\n", weak.m_ManagedPtr);

            Vector3 v2(3.f, 4.f, 5.f);
            System::Object strong(backend::Object::Box(v3Class, &v2));
            strongHandle = std::move(GCHandle::New(strong));
            printf("Strong obj ptr %p\n", strong.m_ManagedPtr);
        }

        if (ImGui::Button("Print handles"))
        {
            printf("Weak handle %d\n", weakHandle.m_Handle);
            printf("Strong handle %d\n", strongHandle.m_Handle);
        }

        if (ImGui::Button("Print objects from handles"))
        {
            printf("Weak object %p\n", weakHandle.GetTarget().m_ManagedPtr);
            printf("Strong object %p\n", strongHandle.GetTarget().m_ManagedPtr);
        }
    }
}

void InitializeMenu()
{
    HaxSdk::Log("HELLO!");
}

static void Start() 
{
    HaxSdk::InitLogger(true);
    HaxSdk::InitBackend();
    HaxSdk::AttachThread();
    HaxSdk::InitSystem();

    v3Class = backend::ReflectionType::GetClass(System::AppDomain::GetRootDomain().Load("UnityEngine.CoreModule", true).GetType("UnityEngine", "Vector3").m_Type);
    HaxSdk::ImplementImGui(GraphicsApi_DirectX11, InitializeMenu, nullptr, RenderMenu);
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
    HaxSdk::SetCheatHandle((HANDLE)module);
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}