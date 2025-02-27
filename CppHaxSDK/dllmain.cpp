#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#include "haxsdk.h"
#include "logger/Logger.h"
#include "haxsdk_gui.h"
#include "unity/haxsdk_unity.h"

#include "third_party/imgui/imgui.h"

void HaxSdk::DoOnceBeforeRendering() {

}

static Unity::GameObject* go = nullptr;

void HaxSdk::RenderMenu() {
    ImGui::Begin("Name");
    if (go) {
        if (ImGui::Button("Toggle light")) {
            go->SetActive(!go->GetActive());
        }
    }
    ImGui::End();
}

void HaxSdk::RenderBackground() {

}

static void Start() {
	HaxSdk::InitLogger(true);
	HaxSdk::InitializeCore();
    HaxSdk::ImplementImGui(GraphicsApi_Any);

    auto pPlayer = *(Unity::Component**)Class::Find("Assembly-CSharp", "", "GameManager")->FindStaticField("m_vpFPSPlayer");

    go = Unity::GameObject::New("LightAround");
    go->GetTransform()->SetParent(pPlayer->GetTransform());
    Unity::Vector3 lightPos = pPlayer->GetTransform()->GetPosition();
    lightPos.y += 2.f;
    go->GetTransform()->SetPosition(lightPos);
    go->SetActive(true);

    auto pLight = (Unity::Light*)go->AddComponent(Unity::Light::GetClass()->GetSystemType());
    pLight->SetIntensity(.5f);
    pLight->SetRange(1000.f);
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	HaxSdk::GetGlobals().cheatModule = (void*)(module);
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}