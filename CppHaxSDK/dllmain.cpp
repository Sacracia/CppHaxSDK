#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <format>

#include "unity/haxsdk_unity.h"
#include "haxsdk_logger.h"
#include "haxsdk_gui.h"
//#include "haxsdk_tools.h"

using namespace System;

static FieldInfo EnemyDirector_instance("Assembly-CSharp", "", "EnemyDirector", "instance");
static FieldInfo EnemyDirector_enemiesSpawned("Assembly-CSharp", "", "EnemyDirector", "enemiesSpawned");
static FieldInfo EnemyParent_Enemy("Assembly-CSharp", "", "EnemyParent", "Enemy");

static MethodInfo Camera_WorldToScreenPoint("UnityEngine.CoreModule", "UnityEngine", "Camera", "WorldToScreenPoint", "UnityEngine.Vector3(UnityEngine.Vector3)");

struct Enemy : Unity::Behaviour
{
    
};

struct EnemyParent : Unity::Behaviour
{
    Enemy enemy()
    {
        CHECK_NULL();
        return *(Enemy*)EnemyParent_Enemy.GetValuePtr(*this);
    }
};

struct EnemyDirector : Unity::Behaviour
{
    static EnemyDirector instance() 
    { 
        return *(EnemyDirector*)EnemyDirector_instance.GetValuePtr(null); 
    }
    
    System::List<EnemyParent> enemiesSpawned()
    { 
        CHECK_NULL();
        return *(System::List<EnemyParent>*)EnemyDirector_enemiesSpawned.GetValuePtr(*this);
    }
};

static bool useEsp = false;

void ESP()
{
    if (useEsp)
    {
        Unity::Camera cam = Unity::Camera::GetMain();
        float screenHeight = (float)Unity::Screen::GetHeight();
        auto enemies = EnemyDirector::instance().enemiesSpawned();
        for (auto& enemy : enemies)
        {
            printf("Contains %d\n", enemies.Contains(enemy));
        }
    }
}

void RenderMenu()
{
    ImGui::ShowDemoWindow();
}

void Init()
{
}

static void Start() 
{
    HaxSdk::InitLogger(true);

    HaxSdk::InitUnity();

    unsafe::Thread* thread = unsafe::Thread::Attach();
    
    HaxSdk::ImplementImGui(GraphicsApi_DirectX11, nullptr, nullptr, RenderMenu);
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
    HaxSdk::SetCheatHandle((HANDLE)module);
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}