#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <format>

#include "haxsdk.h"
#include "haxsdk_gui.h"

#include "third_party/imgui/imgui.h"
#include "third_party/detours/x64/detours.h"

static Method<void(void*)> EventSystem_Update;
static Method<void*()> SemiFunc_MainCamera;
static Method<void(void*, bool, bool, int32_t)> RunManager_ChangeLevel;
static bool g_ChangeLevel = false;

void Hooked__EventSystem_Update(void* __this)
{
    if (g_ChangeLevel)
    {
        g_ChangeLevel = false;

        MonoException* ex = nullptr;
        void** runManager = HaxSdk::FindStaticField<void*>("Assembly-CSharp", "", "RunManager", "instance");
        RunManager_ChangeLevel.Thunk(*runManager, true, false, 0, &ex);

        MonoThread* thread = mono_thread_current();
        if (!thread) 
        {
            HaxSdk::Log("NO THREAD");
        }

        ex = nullptr;
        void* camera = SemiFunc_MainCamera.Thunk(&ex);
        if (ex)
            HaxSdk::Log("ERROR");
        else
            HaxSdk::Log(std::format("Camera {}", camera));
    }

    EventSystem_Update.HookOrig(__this);
}

void Initialize()
{
    HaxSdk::AttachThread();

    SemiFunc_MainCamera = HaxSdk::FindMethod("Assembly-CSharp", "", "SemiFunc", "MainCamera");
    EventSystem_Update = HaxSdk::FindMethod("UnityEngine.UI", "UnityEngine.EventSystems", "EventSystem", "Update");
    RunManager_ChangeLevel = HaxSdk::FindMethod("Assembly-CSharp", "", "RunManager", "ChangeLevel");

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    EventSystem_Update.Hook(Hooked__EventSystem_Update);
    DetourTransactionCommit();
}

void RenderMenu() {
    ImGui::Begin("Window");
    if (ImGui::Button("PRESS"))
    {
        g_ChangeLevel = true;
    }

    ImGui::End();
}

static void Start() {
    HaxSdk::AddMenuRender(RenderMenu);
    HaxSdk::AddInitializer(Initialize);

    HaxSdk::Initialize(true);
    HaxSdk::ImplementImGui(GraphicsApi_Any);
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	HaxSdk::GetGlobals().CheatModule = (void*)(module);
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}