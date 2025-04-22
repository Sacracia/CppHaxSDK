#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <format>

#include "haxsdk.h"
#include "backend/haxsdk_backend.h"
#include "haxsdk_gui.h"

#include "third_party/imgui/imgui.h"
#include "third_party/detours/x64/detours.h"

struct RunManager : HaxObject
{
};

struct Camera : HaxObject
{
};

static HaxMethod<void(void*)> EventSystem_Update;
static HaxMethod<Camera()> SemiFunc_MainCamera;
static HaxMethod<void(RunManager, bool, bool, int32_t)> RunManager_ChangeLevel;
static bool g_ChangeLevel = false;

static void RenderMenu()
{
    if (ImGui::Begin("Window"))
    {
        if (ImGui::Button("123"))
            g_ChangeLevel = true;

        if (ImGui::Button("Camera"))
        {
            Camera camera = SemiFunc_MainCamera.InvokeStatic();
            printf("Camera = %p\n", (void*)camera);
        }

        ImGui::End();
    }
}

void Hooked__EventSystem_Update(void* __this)
{
    if (g_ChangeLevel)
    {
        g_ChangeLevel = false;

        RunManager runManager = *(RunManager*)HaxClass::Find("Assembly-CSharp", "", "RunManager").FindStaticField("instance");
        RunManager_ChangeLevel.Invoke(runManager, true, false, 0);

        SemiFunc_MainCamera.Thunk();
    }

    EventSystem_Update.HookOrig(__this);
}

void Initialize()
{
    HaxSdk::AttachThread();

    SemiFunc_MainCamera = HaxClass::Find("Assembly-CSharp", "", "SemiFunc").FindMethod("MainCamera");
    EventSystem_Update = HaxClass::Find("UnityEngine.UI", "UnityEngine.EventSystems", "EventSystem").FindMethod("Update");
    RunManager_ChangeLevel = HaxClass::Find("Assembly-CSharp", "", "RunManager").FindMethod("ChangeLevel");

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    EventSystem_Update.Hook(Hooked__EventSystem_Update);
    DetourTransactionCommit();
}

static void Start() {
    HaxSdk::AddMenuRender(RenderMenu);
    HaxSdk::AddInitializer(Initialize);

    HaxSdk::Initialize(true);
    //HaxClass klass = HaxClass::Find("mscorlib", "System.Collections", "DictionaryEntry");
    //int32_t _key = HaxClass::Find("mscorlib", "System.Collections", "DictionaryEntry").FindField("_key");
    //int32_t _value = HaxClass::Find("mscorlib", "System.Collections", "DictionaryEntry").FindField("_value");
    //void* EmptyArray = *(void**)HaxClass::Find("mscorlib", "System", "String").FindStaticField("Empty");

    HaxSdk::ImplementImGui(GraphicsApi_DirectX11);
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	HaxSdk::GetGlobals().CheatModule = (void*)(module);
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}