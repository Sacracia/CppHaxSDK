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
#include <d3d10_1.h>
#include <dxgi.h>
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "dxgi.lib")

//directx 11
#include <d3d11.h>
//#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "dxgi.lib")

#include <d3d12.h>
namespace dx12 {
	#include <dxgi1_4.h>
}
#pragma comment(lib, "d3d12.lib")

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
#include "../third_party/imgui/backend/imgui_impl_dx11.h"
#include "../third_party/imgui/backend/imgui_impl_dx12.h"
#include "../third_party/imgui/backend/imgui_impl_win32.h"
#include "../third_party/imgui/backend/imgui_impl_opengl3.h"

using setCursorPos_t		= BOOL(WINAPI*)(int, int);
using clipCursor_t			= BOOL(WINAPI*)(const RECT*);
using swapBuffers_t			= bool(WINAPI*)(HDC);
using reset_t				= HRESULT(WINAPI*)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
using endScene_t			= HRESULT(WINAPI*)(LPDIRECT3DDEVICE9);
using present_t				= HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT);
using resizeBuffers_t		= HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
using setRenderTargets_t	= void(WINAPI*)(ID3D10Device*, UINT, ID3D10RenderTargetView* const*, ID3D10DepthStencilView*);
using setRenderTargets11_t	= void(WINAPI*)(ID3D11DeviceContext*, UINT, ID3D11RenderTargetView* const*, ID3D11DepthStencilView*);
using executeCommandLists_t = void(WINAPI*)(ID3D12CommandQueue*, UINT, ID3D12CommandList*);

static bool						g_visible = true;
static bool						g_d3dlock;
static WNDPROC					oWndproc;
static setCursorPos_t			oSetCursorPos;
static clipCursor_t				oClipCursor;
static swapBuffers_t			oSwapBuffers;
static reset_t					oReset;
static endScene_t				oEndScene;
// directx 10
static present_t				oD3D10Present;
static resizeBuffers_t			oD3D10ResizeBuffers;
static setRenderTargets_t		oD3D10OMSetRenderTargets;
static ID3D10RenderTargetView*	g_pD3D10RenderTarget;
static ID3D10Device*			g_pD3D10Device;
// directx 11
static present_t				oD3D11Present;
static resizeBuffers_t			oD3D11ResizeBuffers;
static setRenderTargets11_t		oD3D11OMSetRenderTargets;
static ID3D11RenderTargetView*	g_pD3D11RenderTarget;
static ID3D11Device*			g_pD3D11Device;
static ID3D11DeviceContext*		g_pD3D11DeviceContext;
// directx 12
namespace dx12 {
	static int const					NUM_BACK_BUFFERS = 3;
	static IDXGIFactory4*				g_dxgiFactory = NULL;
	static ID3D12Device*				g_pd3dDevice = NULL;
	static ID3D12DescriptorHeap*		g_pd3dRtvDescHeap = NULL;
	static ID3D12DescriptorHeap*		g_pd3dSrvDescHeap = NULL;
	static ID3D12CommandQueue*			g_pd3dCommandQueue = NULL;
	static ID3D12GraphicsCommandList*	g_pd3dCommandList = NULL;
	static IDXGISwapChain3*				g_pSwapChain = NULL;
	static ID3D12CommandAllocator*		g_commandAllocators[NUM_BACK_BUFFERS] = { };
	static ID3D12Resource*				g_mainRenderTargetResource[NUM_BACK_BUFFERS] = { };
	static D3D12_CPU_DESCRIPTOR_HANDLE	g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = { };
	static present_t					oD3D12Present;
	static resizeBuffers_t				oD3D12ResizeBuffers;
	static executeCommandLists_t		oD3D12ExecuteCommandLists;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

enum GraphicsAPI { OpenGL, D3D9, D3D10, D3D11, D3D12 };

struct ImGuiContextParams
{
	GraphicsAPI			api;
	LPDIRECT3DDEVICE9	pDevice9;
	HDC					hdc;
	IDXGISwapChain*		pSwapChain;
};

static LRESULT WINAPI	HookedWndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static BOOL WINAPI		HookedSetCursorPos(int X, int Y);
static BOOL WINAPI		HookedClipCursor(const RECT* lpRect);
static void				InitImGuiContext(ImGuiContextParams params);
// opengl
static void				OpenGL_Hook();
static bool WINAPI		OpenGL_HookedSwapBuffers(HDC hdc);
// directx 9
static void				D3D9_Hook();
static HRESULT WINAPI	D3D9_HookedReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
static HRESULT WINAPI	D3D9_HookedEndScene(LPDIRECT3DDEVICE9 pDevice);
// directx 10
static void				D3D10_Hook();
static HRESULT WINAPI	D3D10_HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
static HRESULT WINAPI	D3D10_HookedPresent(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);
static void				D3D10_CreateRenderTarget(IDXGISwapChain* pSwapChain);
static void				D3D10_HookedOMSetRenderTargets(ID3D10Device*, UINT, ID3D10RenderTargetView* const*, ID3D10DepthStencilView*);
// directx 11
static void				D3D11_Hook();
static HRESULT WINAPI	D3D11_HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
static HRESULT WINAPI	D3D11_HookedPresent(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);
static void				D3D11_CreateRenderTarget(IDXGISwapChain* pSwapChain);
static void				D3D11_HookedOMSetRenderTargets(ID3D11DeviceContext*, UINT, ID3D11RenderTargetView* const*, ID3D11DepthStencilView*);
// directx 12
namespace dx12 {
	static void				D3D12_Hook();
	static void				D3D12_Init(IDXGISwapChain*);
	static HRESULT WINAPI	D3D12_HookedResizeBuffers(dx12::IDXGISwapChain3* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
	static HRESULT WINAPI	D3D12_HookedPresent(dx12::IDXGISwapChain3* pSwapChain, UINT syncInterval, UINT flags);
	static void				D3D12_CreateRenderTarget(dx12::IDXGISwapChain3* pSwapChain);
	static void	WINAPI		D3D12_HookedExecuteCommandLists(ID3D12CommandQueue* pCommandQueue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists);
}

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
				//OpenGL_Hook();
			}
			if (moduleName == "d3d9.dll") {
				LOG_INFO << "DIRECTX9 graphics api found" << LOG_FLUSH;
				//D3D9_Hook();
			}
			if (moduleName == "d3d11.dll") {
				LOG_INFO << "DIRECTX11 graphics api found" << LOG_FLUSH;
				//D3D11_Hook();
			}
			if (moduleName == "d3d10.dll") {
				LOG_INFO << "DIRECTX10 graphics api found" << LOG_FLUSH;
				//D3D10_Hook();
			}
			if (moduleName == "d3d12.dll") {
				LOG_INFO << "DIRECTX12 graphics api found" << LOG_FLUSH;
				dx12::D3D12_Hook();
			}
			if (moduleName == "vulkan-1.dll") {
				LOG_INFO << "VULKAN graphics api found" << LOG_FLUSH;
			}
		} while (Module32Next(snapshot, &me));
	}

	CloseHandle(snapshot);
}

static LRESULT WINAPI HookedWndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	ImGuiIO& io = ImGui::GetIO();
	POINT position;
	GetCursorPos(&position);
	ScreenToClient(hWnd, &position);
	io.MousePos.x = (float)position.x;
	io.MousePos.y = (float)position.y;


	if (uMsg == WM_KEYUP && wParam == VK_OEM_3) { 
		g_visible = !g_visible; 
	}

	if (g_visible) {
		RECT rect;
		GetWindowRect(hWnd, &rect);
		HookedClipCursor(&rect);

		io.MouseDrawCursor = true;

		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		switch (uMsg) {
		case WM_MOUSEMOVE:
		case WM_MOUSEACTIVATE:
		case WM_MOUSEHOVER:
		case WM_NCMOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
		case WM_INPUT:
		case WM_TOUCH:
		case WM_POINTERDOWN:
		case WM_POINTERUP:
		case WM_POINTERUPDATE:
		case WM_NCMOUSELEAVE:
		case WM_MOUSELEAVE:
		case WM_SETCURSOR:
		case WM_SIZE:
		case WM_MOVE:
		case WM_KEYDOWN:
		case WM_KEYUP:
			return true;
		}
	}
	io.MouseDrawCursor = false;
	return CallWindowProc(oWndproc, hWnd, uMsg, wParam, lParam);
}

static BOOL WINAPI HookedSetCursorPos(int X, int Y) {
	return g_visible ? true : oSetCursorPos(X, Y);
}

static BOOL WINAPI HookedClipCursor(const RECT* lpRect) {
	return oClipCursor(g_visible ? NULL : lpRect);
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
	if (params.api == GraphicsAPI::D3D10) {
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		params.pSwapChain->GetDevice(IID_PPV_ARGS(&g_pD3D10Device));
		params.pSwapChain->GetDesc(&swapChainDesc);
		hwnd = swapChainDesc.OutputWindow;
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX10_Init(g_pD3D10Device);
	}
	if (params.api == GraphicsAPI::D3D11) {
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		params.pSwapChain->GetDevice(IID_PPV_ARGS(&g_pD3D11Device));
		params.pSwapChain->GetDesc(&swapChainDesc);
		g_pD3D11Device->GetImmediateContext(&g_pD3D11DeviceContext);
		hwnd = swapChainDesc.OutputWindow;
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(g_pD3D11Device, g_pD3D11DeviceContext);
	}
	if (params.api == GraphicsAPI::D3D12) {
		dx12::D3D12_Init(params.pSwapChain);
	}

	ImGuiIO& io = ImGui::GetIO();
	io.WantCaptureMouse = true;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	oWndproc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)HookedWndproc);

	oClipCursor = (clipCursor_t)GetProcAddress(GetModuleHandleA("user32.dll"), "ClipCursor");
	DetourTransactionBegin();
	DetourAttach(&(PVOID&)oClipCursor, HookedClipCursor);
	DetourTransactionCommit();
}

//-----------------------------------------------------------------------------
// [SECTION] OPENGL
//-----------------------------------------------------------------------------

static void OpenGL_Hook() {
	HMODULE module = GetModuleHandleA("opengl32.dll");

	oSwapBuffers = (swapBuffers_t)GetProcAddress(module, "wglSwapBuffers");
	if (oSwapBuffers == 0) {
		LOG_ERROR << "[OPENGL] Unable to find wglSwapBuffers. Hook not installed" << LOG_FLUSH;
		return;
	}
	LOG_INFO << "[OPENGL] wglSwapBuffers address is " << oSwapBuffers << LOG_FLUSH;
	DetourTransactionBegin();
	DetourAttach(&(PVOID&)oSwapBuffers, OpenGL_HookedSwapBuffers);
	DetourTransactionCommit();
}

static bool WINAPI OpenGL_HookedSwapBuffers(HDC hdc) {
	static bool inited = false;
	if (!inited) {
		inited = true;
		ImGuiContextParams params = { GraphicsAPI::OpenGL, nullptr, hdc, nullptr };
		InitImGuiContext(params);
		LOG_WARNING << "GAME USES OPENGL" << LOG_FLUSH;
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

//-----------------------------------------------------------------------------
// [SECTION] DIRECTX 9
//-----------------------------------------------------------------------------

static void D3D9_Hook() {
	HMODULE module = ::GetModuleHandle("d3d9.dll");

	LPDIRECT3D9 d3d9 = ::Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d9 == NULL) {
		LOG_ERROR << "[D3D9] Direct3DCreate9 failed. Hook not installed" << LOG_FLUSH;
		return;
	}
	LPDIRECT3DDEVICE9 dummyDev = NULL;

	D3DPRESENT_PARAMETERS d3dpp = {};
	d3dpp.Windowed = false;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = ::GetConsoleWindow();
	HRESULT result = d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &dummyDev);
	if (result != D3D_OK) {
		LOG_ERROR << "[D3D9] IDirect3D9::CreateDevice returned " << result << ". Hook not installed" << LOG_FLUSH;
		d3d9->Release();
		return;
	}

	void** pVTable = *(void***)(dummyDev);
	oEndScene = (endScene_t)pVTable[42];
	oReset = (reset_t)pVTable[16];

	LOG_INFO << "[D3D9] IDirect3DDevice9::EndScene address is " << oEndScene << LOG_FLUSH;
	LOG_INFO << "[D3D9] IDirect3DDevice9::Reset address is " << oReset << LOG_FLUSH;

	dummyDev->Release();
	d3d9->Release();

	DetourTransactionBegin();
	DetourAttach(&(PVOID&)oEndScene, D3D9_HookedEndScene);
	DetourAttach(&(PVOID&)oReset, D3D9_HookedReset);
	DetourTransactionCommit();
}

static HRESULT WINAPI D3D9_HookedEndScene(LPDIRECT3DDEVICE9 pDevice) {
	static bool inited = false;
	if (!inited) {
		inited = true;
		ImGuiContextParams params = { GraphicsAPI::D3D9, pDevice, 0, nullptr };
		InitImGuiContext(params);
		LOG_WARNING << "GAME USES DIRECTX9" << LOG_FLUSH;
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

static HRESULT WINAPI D3D9_HookedReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT result = oReset(pDevice, pPresentationParameters);
	ImGui_ImplDX9_CreateDeviceObjects();
	LOG_INFO << "[D3D9] Swap chain was reset" << LOG_FLUSH;
	return result;
}

//-----------------------------------------------------------------------------
// [SECTION] DIRECTX 10
//-----------------------------------------------------------------------------

static void D3D10_Hook() {
	HMODULE module = GetModuleHandle("d3d10.dll");

	DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = ::GetConsoleWindow();
	swapChainDesc.SampleDesc.Count = 1;

	IDXGISwapChain* pSwapChain;
	ID3D10Device* pDevice;
	HRESULT result = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_NULL, NULL, 0, D3D10_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice);
	if (result != S_OK) {
		LOG_ERROR << "[D3D10] D3D10CreateDeviceAndSwapChain returned " << result << ". Hook not installed" << LOG_FLUSH;
		return;
	}

	void** pVTable = *reinterpret_cast<void***>(pSwapChain);
	oD3D10Present = (present_t)pVTable[8];
	oD3D10ResizeBuffers = (resizeBuffers_t)pVTable[13];
	pVTable = *reinterpret_cast<void***>(pDevice);
	oD3D10OMSetRenderTargets = (setRenderTargets_t)(pVTable[24]);

	LOG_INFO << "[D3D10] IDXGISwapChain::Present address is " << oD3D10Present << LOG_FLUSH;
	LOG_INFO << "[D3D10] IDXGISwapChain::ResizeBuffers address is " << oD3D10ResizeBuffers << LOG_FLUSH;
	LOG_INFO << "[D3D10] ID3D10Device::OMSetRenderTargets address is " << oD3D10OMSetRenderTargets << LOG_FLUSH;

	pSwapChain->Release();
	pDevice->Release();

	/*DetourTransactionBegin();
	DetourAttach(&(PVOID&)oD3D10Present, D3D10_HookedPresent);
	DetourAttach(&(PVOID&)oD3D10ResizeBuffers, D3D10_HookedResizeBuffers);
	DetourAttach(&(PVOID&)oD3D10OMSetRenderTargets, D3D10_HookedOMSetRenderTargets);
	DetourTransactionCommit();*/
}

static HRESULT WINAPI D3D10_HookedPresent(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags) {
	static bool inited = false;
	if (!inited) {
		inited = true;
		ImGuiContextParams params = { GraphicsAPI::D3D10, nullptr, 0, pSwapChain };
		InitImGuiContext(params);
		LOG_INFO << "[D3D10] Present inited" << LOG_FLUSH;
	}
	if (!g_pD3D10RenderTarget) {
		D3D10_CreateRenderTarget(pSwapChain);
	}
	ImGui_ImplDX10_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (g_visible) {
		ImGui::ShowDemoWindow();
	}
	ImGui::EndFrame();
	ImGui::Render();
	g_pD3D10Device->OMSetRenderTargets(1, &g_pD3D10RenderTarget, nullptr);
	ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());

	return oD3D10Present(pSwapChain, syncInterval, flags);
}

static HRESULT WINAPI D3D10_HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, 
												UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
	if (g_pD3D10RenderTarget) {
		g_pD3D10RenderTarget->Release();
		g_pD3D10RenderTarget = nullptr;
	}
	LOG_INFO << "[D3D10] Buffers were resized" << LOG_FLUSH;
	return oD3D10ResizeBuffers(pSwapChain, bufferCount, width, height, newFormat, swapChainFlags);
}

static void D3D10_CreateRenderTarget(IDXGISwapChain* pSwapChain) {
	ID3D10Texture2D* pBackBuffer = NULL;
	pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (pBackBuffer) {
		DXGI_SWAP_CHAIN_DESC sd;
		pSwapChain->GetDesc(&sd);
		g_pD3D10Device->CreateRenderTargetView(pBackBuffer, nullptr, &g_pD3D10RenderTarget);
		pBackBuffer->Release();
	}
}

static void	D3D10_HookedOMSetRenderTargets(ID3D10Device* pDevice, UINT numViews, ID3D10RenderTargetView* const* ppRenderTargetViews,
										   ID3D10DepthStencilView* pDepthStencilView) {
	static bool inited = false;
	if (!inited) {
		inited = true;
		LOG_WARNING << "GAME USES DIRECTX10" << LOG_FLUSH;
	}
	oD3D10OMSetRenderTargets(pDevice, numViews, ppRenderTargetViews, pDepthStencilView);
}

//-----------------------------------------------------------------------------
// [SECTION] DIRECT X 11
//-----------------------------------------------------------------------------

static void	D3D11_Hook() {
	HMODULE module = GetModuleHandle("d3d11.dll");

	DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = ::GetConsoleWindow();
	swapChainDesc.SampleDesc.Count = 1;

	const D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};
	IDXGISwapChain* pSwapChain;
	ID3D11Device* pDevice;
	HRESULT result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_NULL, NULL, 0, featureLevels, 2, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, nullptr, &g_pD3D11DeviceContext);
	if (result != S_OK) {
		LOG_ERROR << "[D3D11] D3D11CreateDeviceAndSwapChain returned " << result << ". Hook not installed" << LOG_FLUSH;
		return;
	}

	void** pVTable = *(void***)(pSwapChain);
	oD3D11Present = (present_t)pVTable[8];
	oD3D11ResizeBuffers = (resizeBuffers_t)pVTable[13];
	pVTable = *(void***)(g_pD3D11DeviceContext);
	oD3D11OMSetRenderTargets = (setRenderTargets11_t)pVTable[33];

	pSwapChain->Release();
	pDevice->Release();

	LOG_INFO << "[D3D11] ID3D11DeviceContext::OMSetRenderTargets address is " << oD3D11OMSetRenderTargets << LOG_FLUSH;
	LOG_INFO << "[D3D11] IDXGISwapChain::Present address is " << oD3D11Present << LOG_FLUSH;
	LOG_INFO << "[D3D11] IDXGISwapChain::ResizeBuffers address is " << oD3D11ResizeBuffers << LOG_FLUSH;

	/*DetourTransactionBegin();
	DetourAttach(&(PVOID&)oD3D11OMSetRenderTargets, D3D11_HookedOMSetRenderTargets);
	DetourTransactionCommit();*/
}

static HRESULT WINAPI D3D11_HookedPresent(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags) {
	static bool inited = false;
	if (!inited) {
		inited = true;
		ImGuiContextParams params = { GraphicsAPI::D3D11, nullptr, 0, pSwapChain };
		InitImGuiContext(params);
		LOG_INFO << "[D3D11] Present inited" << LOG_FLUSH;
	}
	if (!g_pD3D11RenderTarget) {
		D3D11_CreateRenderTarget(pSwapChain);
	}
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (g_visible) {
		ImGui::ShowDemoWindow();
	}
	ImGui::EndFrame();
	ImGui::Render();
	g_pD3D11DeviceContext->OMSetRenderTargets(1, &g_pD3D11RenderTarget, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return oD3D11Present(pSwapChain, syncInterval, flags);
}

static HRESULT WINAPI D3D11_HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width,
												UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
	if (g_pD3D11RenderTarget) {
		g_pD3D11RenderTarget->Release();
		g_pD3D11RenderTarget = nullptr;
	}
	LOG_INFO << "[D3D11] Buffers were resized" << LOG_FLUSH;
	return oD3D11ResizeBuffers(pSwapChain, bufferCount, width, height, newFormat, swapChainFlags);
}

static void	D3D11_CreateRenderTarget(IDXGISwapChain* pSwapChain) {
	ID3D11Texture2D* pBackBuffer = NULL;
	pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (pBackBuffer && g_pD3D11Device) {
		DXGI_SWAP_CHAIN_DESC sd;
		pSwapChain->GetDesc(&sd);
		g_pD3D11Device->CreateRenderTargetView(pBackBuffer, nullptr, &g_pD3D11RenderTarget);
		pBackBuffer->Release();
	}
}

static void	D3D11_HookedOMSetRenderTargets(ID3D11DeviceContext* pDevice, UINT numViews, ID3D11RenderTargetView* const* ppRenderTargetViews,
										   ID3D11DepthStencilView* pDepthStencilView) {
	static bool inited = false;
	if (!inited) {
		inited = true;
		LOG_WARNING << "GAME USES DIRECTX11" << LOG_FLUSH;
		DetourTransactionBegin();
		DetourAttach(&(PVOID&)oD3D11Present, D3D11_HookedPresent);
		DetourAttach(&(PVOID&)oD3D11ResizeBuffers, D3D11_HookedResizeBuffers);
		DetourTransactionCommit();
	}
	oD3D11OMSetRenderTargets(pDevice, numViews, ppRenderTargetViews, pDepthStencilView);
}

//-----------------------------------------------------------------------------
// [SECTION] DIRECT X 12
//-----------------------------------------------------------------------------

namespace dx12 {
	static void	D3D12_Hook() {
		DXGI_SWAP_CHAIN_DESC1 sd = { };
		sd.BufferCount = NUM_BACK_BUFFERS;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SampleDesc.Count = 1;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		// Create device
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
		if (D3D12CreateDevice(NULL, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
			return;

		D3D12_COMMAND_QUEUE_DESC desc = { };
		if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
			return;

		IDXGISwapChain1* swapChain1 = NULL;
		if (CreateDXGIFactory1(IID_PPV_ARGS(&g_dxgiFactory)) != S_OK)
			return;
		if (g_dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, ::GetConsoleWindow(), &sd, NULL, NULL, &swapChain1) != S_OK)
			return;
		if (swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
			return;
		swapChain1->Release();

		void** pVTable = *reinterpret_cast<void***>(g_pSwapChain);
		oD3D12Present = (present_t)pVTable[8];
		oD3D12ResizeBuffers = (resizeBuffers_t)pVTable[13];
		void** pCommandQueueVTable = *reinterpret_cast<void***>(g_pd3dCommandQueue);
		oD3D12ExecuteCommandLists = (executeCommandLists_t)pCommandQueueVTable[10];

		LOG_INFO << "[D3D12] Present address is " << oD3D12Present << LOG_FLUSH;
		LOG_INFO << "[D3D12] oResizeBuffers address is " << oD3D12ResizeBuffers << LOG_FLUSH;
		LOG_INFO << "[D3D12] oD3D12ExecuteCommandLists address is " << oD3D12ResizeBuffers << LOG_FLUSH;

		DetourTransactionBegin();
		DetourAttach(&(PVOID&)oD3D12Present, D3D12_HookedPresent);
		DetourAttach(&(PVOID&)oD3D12ResizeBuffers, D3D12_HookedResizeBuffers);
		DetourAttach(&(PVOID&)oD3D12ExecuteCommandLists, D3D12_HookedExecuteCommandLists);
		//DetourAttach(&(PVOID&)oD3D10OMSetRenderTargets, D3D10_HookedOMSetRenderTargets);
		DetourTransactionCommit();
	}

	static void D3D12_Init(IDXGISwapChain* pSwapChain) {
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = { };
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.NumDescriptors = NUM_BACK_BUFFERS;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 1;
			if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
				return;

			SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
			for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i) {
				g_mainRenderTargetDescriptor[i] = rtvHandle;
				rtvHandle.ptr += rtvDescriptorSize;
			}
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = { };
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.NumDescriptors = 1;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
				return;
		}

		for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i)
			if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_commandAllocators[i])) != S_OK)
				return;

		if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_commandAllocators[0], NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
			g_pd3dCommandList->Close() != S_OK)
			return;

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		pSwapChain->GetDesc(&swapChainDesc);
		HWND hwnd = swapChainDesc.OutputWindow;

		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX12_Init(g_pd3dDevice, NUM_BACK_BUFFERS,
			DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
			g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
	}

	static HRESULT WINAPI D3D12_HookedResizeBuffers(dx12::IDXGISwapChain3* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
		for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i) {
			if (g_mainRenderTargetResource[i]) {
				g_mainRenderTargetResource[i]->Release();
				g_mainRenderTargetResource[i] = NULL;
			}
		}

		return oD3D12ResizeBuffers(pSwapChain, bufferCount, width, height, newFormat, swapChainFlags);
	}

	static HRESULT WINAPI D3D12_HookedPresent(dx12::IDXGISwapChain3* pSwapChain, UINT syncInterval, UINT flags) {
		static bool inited = false;
		if (!inited) {
			inited = true;
			ImGuiContextParams params = { GraphicsAPI::D3D12, nullptr, 0, pSwapChain };
			InitImGuiContext(params);
			LOG_INFO << "[D3D12] Present inited" << LOG_FLUSH;
		}

		LOG_INFO << "0" << LOG_FLUSH;

		if (!g_mainRenderTargetResource[0]) {
			D3D12_CreateRenderTarget(pSwapChain);
		}

		LOG_INFO << "1" << LOG_FLUSH;

		if (ImGui::GetCurrentContext() && g_pd3dCommandQueue && g_mainRenderTargetResource[0]) {
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			ImGui::ShowDemoWindow();
			ImGui::EndFrame();
			ImGui::Render();

			LOG_INFO << "2" << LOG_FLUSH;
			UINT backBufferIdx = pSwapChain->GetCurrentBackBufferIndex();
			ID3D12CommandAllocator* commandAllocator = g_commandAllocators[backBufferIdx];
			commandAllocator->Reset();

			D3D12_RESOURCE_BARRIER barrier = { };
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			g_pd3dCommandList->Reset(commandAllocator, NULL);
			g_pd3dCommandList->ResourceBarrier(1, &barrier);

			LOG_INFO << "3" << LOG_FLUSH;

			g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL);
			g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			g_pd3dCommandList->ResourceBarrier(1, &barrier);
			g_pd3dCommandList->Close();

			LOG_INFO << "4" << LOG_FLUSH;

			g_pd3dCommandQueue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&g_pd3dCommandList));
		}

		LOG_INFO << "5" << LOG_FLUSH;

		return oD3D12Present(pSwapChain, syncInterval, flags);
	}

	static void	D3D12_CreateRenderTarget(dx12::IDXGISwapChain3* pSwapChain) {
		for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i) {
			ID3D12Resource* pBackBuffer = NULL;
			pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
			if (pBackBuffer) {
				DXGI_SWAP_CHAIN_DESC sd;
				pSwapChain->GetDesc(&sd);

				g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, g_mainRenderTargetDescriptor[i]);
				g_mainRenderTargetResource[i] = pBackBuffer;
			}
		}
	}

	static void	WINAPI D3D12_HookedExecuteCommandLists(ID3D12CommandQueue* pCommandQueue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists) {
		if (!g_pd3dCommandQueue) {
			LOG_INFO << "COMMAND QUEUE" << LOG_FLUSH;
			g_pd3dCommandQueue = pCommandQueue;
		}

		return oD3D12ExecuteCommandLists(pCommandQueue, NumCommandLists, ppCommandLists);
	}
}