#include "directx9.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <d3d9.h>
#include <logger.h>
#include <detours.h>

using endScene_t = HRESULT(WINAPI*)(LPDIRECT3DDEVICE9);
static endScene_t oEndScene;

HRESULT WINAPI HookedEndScene(LPDIRECT3DDEVICE9 device) {
	static bool inited = false;
	if (!inited) {
		inited = true;
		LOG_INFO << "Hello from hooked EndScene!" << LOG_FLUSH;
	}
	return oEndScene(device);
}

namespace haxsdk::directx9 {
	void HookDirectx9() {
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
		
		LOG_INFO << "IDirect3DDevice9::EndScene " << oEndScene << LOG_FLUSH;

		dummyDev->Release();
		d3d9->Release();

		DetourTransactionBegin();
		DetourAttach(&(PVOID&)oEndScene, HookedEndScene);
		DetourTransactionCommit();
	}
}