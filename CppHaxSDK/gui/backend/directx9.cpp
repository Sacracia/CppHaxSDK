#include "directx9.h"

#include <d3d9.h>
#include <detours.h>
#include <backend/imgui_impl_dx9.h>
#include <backend/imgui_impl_win32.h>

#include <logger.h>

using reset_t = HRESULT(WINAPI*)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS);
static reset_t oReset;

static HRESULT WINAPI HookedReset(LPDIRECT3DDEVICE9 device, D3DPRESENT_PARAMETERS params) {
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT result = oReset(device, params);
	ImGui_ImplDX9_CreateDeviceObjects();
	return result;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static WNDPROC oWndproc;
static LRESULT WINAPI HookedWndproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	return CallWindowProc(oWndproc, hWnd, msg, wParam, lParam);
}

using endScene_t = HRESULT(WINAPI*)(LPDIRECT3DDEVICE9);
static endScene_t oEndScene;

static HRESULT WINAPI HookedEndScene(LPDIRECT3DDEVICE9 device) {
	static bool inited = false;
	if (!inited) {
		LOG_INFO << "Hello from hooked EndScene!" << LOG_FLUSH;
		inited = true;
		D3DDEVICE_CREATION_PARAMETERS params;
		device->GetCreationParameters(&params);
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(params.hFocusWindow);
		ImGui_ImplDX9_Init(device);
		oWndproc = (WNDPROC)SetWindowLongPtr(params.hFocusWindow, GWLP_WNDPROC, (LONG_PTR)HookedWndproc);
	}
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();
	ImGui::ShowDemoWindow();
	ImGui::EndFrame();

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	return oEndScene(device);
}

namespace haxsdk::directx9 {
	void HookDirectx9(HWND hwnd) {
		HMODULE module = ::GetModuleHandle("d3d9.dll");
		if (module == 0) {
			LOG_ERROR << "Unable to get d3d9.dll handle" << LOG_FLUSH;
			return;
		}

		LPDIRECT3D9 d3d9 = ::Direct3DCreate9(D3D_SDK_VERSION);
		if (d3d9 == NULL) {
			LOG_ERROR << "Direct3DCreate9 returned NULL" << LOG_FLUSH;
			return;
		}
		LPDIRECT3DDEVICE9 dummyDev = NULL;

		D3DPRESENT_PARAMETERS d3dpp = {};
		d3dpp.Windowed = false;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow = ::GetConsoleWindow();
		HRESULT result = d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &dummyDev);
		if (result != D3D_OK) {
			LOG_ERROR << "IDirect3D9::CreateDevice returned " << result << LOG_FLUSH;
			d3d9->Release();
			return;
		}

		void** pVTable = *reinterpret_cast<void***>(dummyDev);
		oEndScene = (endScene_t)pVTable[42];
		oReset = (reset_t)pVTable[16];
		
		LOG_INFO << "IDirect3DDevice9::EndScene " << oEndScene << LOG_FLUSH;
		LOG_INFO << "IDirect3DDevice9::Reset " << oReset << LOG_FLUSH;

		dummyDev->Release();
		d3d9->Release();

		DetourTransactionBegin();
		DetourAttach(&(PVOID&)oEndScene, HookedEndScene);
		DetourAttach(&(PVOID&)oReset, HookedReset);
		DetourTransactionCommit();
	}
}