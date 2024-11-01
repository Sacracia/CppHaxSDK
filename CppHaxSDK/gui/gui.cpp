#include "gui.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <TlHelp32.h>

// opengl
#pragma comment(lib, "OpenGL32.lib")

// directx 9
#include <d3d9.h>
#pragma comment(lib, "D3d9.lib")

//directx 10
#include <d3d10.h>
#include <dxgi.h>
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "dxgi.lib")

#include "../logger/logger.h"

#ifdef _WIN64
#include "../third_party/detours/x64/detours.h"
#pragma comment(lib, "third_party/detours/x64/detours.lib")
#else
#include "../third_party/detours/x86/detours.h"
#pragma comment(lib, "third_party/detours/x86/detours.lib")
#endif

#include "../third_party/imgui/imgui.h"
#include "../third_party/imgui/imgui_internal.h"
#include "../third_party/imgui/backend/imgui_impl_dx9.h"
#include "../third_party/imgui/backend/imgui_impl_dx10.h"
#include "../third_party/imgui/backend/imgui_impl_win32.h"
#include "../third_party/imgui/backend/imgui_impl_opengl3.h"

using swapBuffers_t		= bool(WINAPI*)(HDC);
using reset_t			= HRESULT(WINAPI*)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
using endScene_t		= HRESULT(WINAPI*)(LPDIRECT3DDEVICE9);
using present_t			= HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT);
using resizeBuffers_t	= HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);

static WNDPROC					oWndproc;
static swapBuffers_t			oSwapBuffers;
static reset_t					oReset;
static endScene_t				oEndScene;
static present_t				oPresent10;
static resizeBuffers_t			oResizeBuffers;
static bool						g_visible = true;
static ID3D10RenderTargetView*	g_mainRenderTargetView;
static ID3D10Device*			g_pDevice;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

enum GraphicsAPI {OpenGL, D3D9, D3D10};

struct ImGuiContextParams
{
	GraphicsAPI			api;
	LPDIRECT3DDEVICE9	pDevice9;
	HDC					hdc;
	IDXGISwapChain*		pSwapChain10;
};

static LRESULT WINAPI HookedWndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static bool WINAPI HookedSwapBuffers(HDC hdc);
static HRESULT WINAPI HookedReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
static HRESULT WINAPI HookedEndScene(LPDIRECT3DDEVICE9 pDevice);
static void HookOpenGL();
static void HookDirectX9();
static void InitImGuiContext(ImGuiContextParams params);
//directx10
static HRESULT WINAPI HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
static HRESULT WINAPI HookedPresent10(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);
static void HookDirectX10();
static void CreateRenderTarget(IDXGISwapChain* pSwapChain);

void haxsdk::ImplementImGui() {
	DWORD processId = GetProcessId(GetCurrentProcess());
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);

	MODULEENTRY32 me{};
	me.dwSize = sizeof(MODULEENTRY32);
	if (Module32First(snapshot, &me)) {
		do {
			std::string moduleName(me.szModule);
			std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), ::tolower);
			if (moduleName == "opengl32.dll") {
				LOG_INFO << "OPENGL32 graphics api found" << LOG_FLUSH;
				HookOpenGL();
			}
			if (moduleName == "d3d9.dll") {
				LOG_INFO << "DIRECTX9 graphics api found" << LOG_FLUSH;
				HookDirectX9();
			}
			else if (moduleName == "d3d10.dll") {
				LOG_INFO << "DIRECTX10 graphics api found" << LOG_FLUSH;
			}
			else if (moduleName == "d3d11.dll") {
				LOG_INFO << "DIRECTX11 graphics api found" << LOG_FLUSH;
			}
			else if (moduleName == "d3d12.dll") {
				LOG_INFO << "DIRECTX12 graphics api found" << LOG_FLUSH;
			}
			else if (moduleName == "vulkan-1.dll") {
				LOG_INFO << "VULKAN graphics api found" << LOG_FLUSH;
			}
		} while (Module32Next(snapshot, &me));
	}

	CloseHandle(snapshot);
}

static void HookOpenGL() {
	HMODULE module = GetModuleHandleA("opengl32.dll");
	if (module == 0) {
		LOG_ERROR << "Unable to get opengl32.dll handle" << LOG_FLUSH;
		return;
	}

	oSwapBuffers = (swapBuffers_t)GetProcAddress(module, "wglSwapBuffers");
	if (oSwapBuffers == 0) {
		LOG_ERROR << "Unable to find wglSwapBuffers" << LOG_FLUSH;
		return;
	}
	LOG_INFO << "wglSwapBuffers address " << oSwapBuffers << LOG_FLUSH;
	DetourTransactionBegin();
	DetourAttach(&(PVOID&)oSwapBuffers, HookedSwapBuffers);
	DetourTransactionCommit();
}

static void HookDirectX9() {
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

	void** pVTable = *(void***)(dummyDev);
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

static void HookDirectX10() {
	HMODULE module = GetModuleHandle("d3d10.dll");

	DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = ::GetConsoleWindow();
	swapChainDesc.SampleDesc.Count = 1;

	IDXGISwapChain* pSwapChain;
	HRESULT hr = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_NULL, NULL, 0, D3D10_SDK_VERSION, &swapChainDesc, &pSwapChain, &g_pDevice);
	if (hr != S_OK) {
		LOG_ERROR << "D3D10CreateDeviceAndSwapChain() failed." << hr << LOG_FLUSH;
		return;
	}

	void** pVTable = *reinterpret_cast<void***>(pSwapChain);
	oPresent10 = (present_t)pVTable[8];
	oResizeBuffers = (resizeBuffers_t)pVTable[13];

	pSwapChain->Release();

	DetourTransactionBegin();
	DetourAttach(&(PVOID&)oPresent10, HookedPresent10);
	DetourAttach(&(PVOID&)oResizeBuffers, HookedResizeBuffers);
	DetourTransactionCommit();
}

static bool WINAPI HookedSwapBuffers(HDC hdc) {
	static bool inited = false;
	if (!inited) {
		inited = true;
		ImGuiContextParams params = { GraphicsAPI::OpenGL, nullptr, hdc, nullptr };
		InitImGuiContext(params);
		LOG_INFO << "Hello from hooked wglSwapBuffers" << LOG_FLUSH;
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (g_visible)
		ImGui::ShowDemoWindow();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return oSwapBuffers(hdc);
}

static HRESULT WINAPI HookedReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT result = oReset(pDevice, pPresentationParameters);
	ImGui_ImplDX9_CreateDeviceObjects();
	LOG_INFO << "Hello from hooked Reset" << LOG_FLUSH;
	return result;
}

static HRESULT WINAPI HookedEndScene(LPDIRECT3DDEVICE9 pDevice) {
	static bool inited = false;
	if (!inited) {
		inited = true;
		ImGuiContextParams params = { GraphicsAPI::D3D9, pDevice, 0, nullptr };
		InitImGuiContext(params);
		LOG_INFO << "Hello from hooked EndScene" << LOG_FLUSH;
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (g_visible) {
		ImGui::ShowDemoWindow();
	}
	ImGui::EndFrame();

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	return oEndScene(pDevice);
}

static HRESULT WINAPI HookedPresent10(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags) {
	static bool inited = false;
	if (!inited) {
		inited = true;
		ImGuiContextParams params = { GraphicsAPI::D3D10, nullptr, 0, pSwapChain };
		InitImGuiContext(params);
		LOG_INFO << "Hello from hooked Present10" << LOG_FLUSH;
	}
	if (!g_mainRenderTargetView) {
		CreateRenderTarget(pSwapChain);
	}
	ImGui_ImplDX10_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();
	ImGui::EndFrame();
	ImGui::Render();
	g_pDevice->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
	ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());
}

static HRESULT WINAPI HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
if (g_mainRenderTargetView) {
		g_mainRenderTargetView->Release();
	}
	CreateRenderTarget(pSwapChain);
}

static void CreateRenderTarget(IDXGISwapChain* pSwapChain) {
	ID3D10Texture2D* pBackBuffer = NULL;
	pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (pBackBuffer) {
		DXGI_SWAP_CHAIN_DESC sd;
		pSwapChain->GetDesc(&sd);
		g_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
		pBackBuffer->Release();
	}
}

static LRESULT WINAPI HookedWndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	ImGuiIO& io = ImGui::GetIO();
	if (uMsg == WM_KEYUP && wParam == VK_OEM_3) {
		g_visible = !g_visible;
	}
	io.MouseDrawCursor = g_visible && io.WantCaptureMouse;

	if (g_visible && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
		return true;
	}

	return CallWindowProc(oWndproc, hWnd, uMsg, wParam, lParam);
}

static void InitImGuiContext(ImGuiContextParams params) {
	static bool inited = false;
	if (inited) { return; }
	inited = true;

	HWND hwnd = 0;
	if (params.api == GraphicsAPI::OpenGL) {
		hwnd = WindowFromDC(params.hdc);
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplOpenGL3_Init();
	}
	if (params.api == GraphicsAPI::D3D9) {
		D3DDEVICE_CREATION_PARAMETERS creationParams;
		params.pDevice9->GetCreationParameters(&creationParams);
		hwnd = creationParams.hFocusWindow;
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX9_Init(params.pDevice9);
	}
	else {
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		params.pSwapChain10->GetDevice(IID_PPV_ARGS(&g_pDevice));
		params.pSwapChain10->GetDesc(&swapChainDesc);
		hwnd = swapChainDesc.OutputWindow;
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX10_Init(g_pDevice);
	}

	ImGuiIO& io = ImGui::GetIO();
	io.MouseDrawCursor = true;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	oWndproc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)HookedWndproc);
}