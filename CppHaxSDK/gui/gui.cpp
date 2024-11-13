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
#pragma comment(lib, "d3d11.lib")

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

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

#include "../logger/logger.h"

#define MONO_GAME_FUNC(r, n, p, s) extern r(*n)p
#include "../mono/mono_game_functions.h"
#undef MONO_GAME_FUNC

using setCursorPos_t            = BOOL(WINAPI*)(int, int);
using clipCursor_t              = BOOL(WINAPI*)(const RECT*);
using swapBuffers_t             = bool(WINAPI*)(HDC);
using reset_t                   = HRESULT(WINAPI*)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
using endScene_t                = HRESULT(WINAPI*)(LPDIRECT3DDEVICE9);
using present_t                 = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT);
using resizeBuffers_t           = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
using setRenderTargets_t        = void(WINAPI*)(ID3D10Device*, UINT, ID3D10RenderTargetView* const*, ID3D10DepthStencilView*);
using setRenderTargets11_t      = void(WINAPI*)(ID3D11DeviceContext*, UINT, ID3D11RenderTargetView* const*, ID3D11DepthStencilView*);
using executeCommandLists_t     = void(WINAPI*)(ID3D12CommandQueue*, UINT, ID3D12CommandList*);
using setRenderTargets12_t      = void(WINAPI*)(ID3D12GraphicsCommandList*, UINT, const D3D12_CPU_DESCRIPTOR_HANDLE*, BOOL, const D3D12_CPU_DESCRIPTOR_HANDLE*);

static bool                             g_visible = true;
static ImplementationDetails            g_details;
static HWND                             g_dummyHWND;
static WNDPROC                          oWndproc;
static setCursorPos_t                   oSetCursorPos;
static clipCursor_t                     oClipCursor;
static swapBuffers_t                    oSwapBuffers;
static present_t                        oPresent;
static resizeBuffers_t                  oResizeBuffers;

namespace dx9 {
    static reset_t                      oReset;
    static endScene_t                   oEndScene;
}
namespace dx10 {
    static ID3D10RenderTargetView*      g_pRenderTarget;
    static ID3D10Device*                g_pDevice;
}
namespace dx11 {
    static ID3D11RenderTargetView*      g_pRenderTarget;
    static ID3D11Device*                g_pDevice;
    static ID3D11DeviceContext*         g_pDeviceContext;
}
namespace dx12 {
    #include <dxgi1_4.h>
    static int const                    NUM_BACK_BUFFERS = 3;
    static IDXGIFactory4*               g_dxgiFactory = NULL;
    static ID3D12Device*                g_pd3dDevice = NULL;
    static ID3D12DescriptorHeap*        g_pd3dRtvDescHeap = NULL;
    static ID3D12DescriptorHeap*        g_pd3dSrvDescHeap = NULL;
    static ID3D12CommandQueue*          g_pd3dCommandQueue = NULL;
    static ID3D12GraphicsCommandList*   g_pd3dCommandList = NULL;
    static IDXGISwapChain3*             g_pSwapChain = NULL;
    static ID3D12CommandAllocator*      g_commandAllocators[NUM_BACK_BUFFERS] = { };
    static ID3D12Resource*              g_mainRenderTargetResource[NUM_BACK_BUFFERS] = { };
    static D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = { };
    static executeCommandLists_t        oExecuteCommandLists;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

enum GraphicsAPI { OpenGL, D3D9, D3D10, D3D11, D3D12 };

struct ImGuiContextParams {
    GraphicsAPI         api;
    LPDIRECT3DDEVICE9   pDevice9;
    HDC                 hdc;
    IDXGISwapChain*     pSwapChain;
};

static LRESULT WINAPI       HookedPresent(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);
static LRESULT WINAPI       HookedWndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static BOOL WINAPI          HookedSetCursorPos(int X, int Y);
static BOOL WINAPI          HookedClipCursor(const RECT* lpRect);
static void                 InitImGuiContext(const ImGuiContextParams& params);

namespace opengl {
    static void             Hook();
    static bool WINAPI      HookedSwapBuffers(HDC hdc);
}
namespace dx9 {
    static void             Hook();
    static HRESULT WINAPI   HookedReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
    static HRESULT WINAPI   HookedEndScene(LPDIRECT3DDEVICE9 pDevice);
}
namespace dx10 {
    static void             Setup();
    static void             Render(IDXGISwapChain* pSwapChain);
    static void             CreateRenderTarget(IDXGISwapChain* pSwapChain);
    static HRESULT WINAPI   HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
}
namespace dx11 {
    static void             Setup();
    static void             Render(IDXGISwapChain* pSwapChain);
    static void             CreateRenderTarget(IDXGISwapChain* pSwapChain);
    static HRESULT WINAPI   HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
}
namespace dx12 {
    static void             Setup();
    static void             Render(IDXGISwapChain3* pSwapChain);
    static void             CreateRenderTarget(IDXGISwapChain* pSwapChain);
    static HRESULT WINAPI   HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, 
                                                UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    static void WINAPI      HookedExecuteCommandLists(ID3D12CommandQueue* pCommandQueue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists);
}

void haxsdk::ImplementImGui(ImplementationDetails& details) {
    g_details = details;
    if (!details.DrawMenuProc) {
        g_details.DrawMenuProc = ImGui::ShowDemoWindow;
    }

    WNDCLASSEX dummyWindow{};
    dummyWindow.cbSize = 0;
    g_dummyHWND = GetConsoleWindow();
    if (!g_dummyHWND) {
        dummyWindow.cbSize = sizeof(WNDCLASSEX);
        dummyWindow.style = CS_HREDRAW | CS_VREDRAW;
        dummyWindow.lpfnWndProc = DefWindowProc;
        dummyWindow.cbClsExtra = 0;
        dummyWindow.cbWndExtra = 0;
        dummyWindow.hInstance = GetModuleHandle(NULL);
        dummyWindow.hIcon = NULL;
        dummyWindow.hCursor = NULL;
        dummyWindow.hbrBackground = NULL;
        dummyWindow.lpszMenuName = nullptr;
        dummyWindow.lpszClassName = "Dummy window";
        dummyWindow.hIconSm = NULL;

        RegisterClassEx(&dummyWindow);
        g_dummyHWND = CreateWindow(dummyWindow.lpszClassName, "Dummy window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, nullptr, nullptr, dummyWindow.hInstance, nullptr);
        LOG_DEBUG << "DUMMY WINDOW CREATED" << LOG_FLUSH;
    }

    DWORD processId = GetProcessId(GetCurrentProcess());
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);

    MODULEENTRY32 me{};
    me.dwSize = sizeof(MODULEENTRY32);
    if (Module32First(snapshot, &me)) {
        do {
            std::string moduleName(me.szModule);
            std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), ::tolower);
            if (moduleName == "opengl32.dll") {
                opengl::Hook();
            }
            if (moduleName == "d3d9.dll") {
                dx9::Hook();
            }
            if (moduleName == "d3d10.dll") {
                dx10::Setup();
            }
            if (moduleName == "d3d11.dll") {
                dx11::Setup();
            }
            if (moduleName == "d3d12.dll") {
                dx12::Setup();
            }
            if (moduleName == "vulkan-1.dll") {
                LOG_INFO << "VULKAN graphics api found" << LOG_FLUSH;
            }
        } while (Module32Next(snapshot, &me));
    }
    CloseHandle(snapshot);

    if (dummyWindow.cbSize > 0) {
        DestroyWindow(g_dummyHWND);
        UnregisterClass(dummyWindow.lpszClassName, dummyWindow.hInstance);
        LOG_DEBUG << "DUMMY WINDOW DESTROYED" << LOG_FLUSH;
    }

    if (oPresent) {
        DetourTransactionBegin();
        DetourAttach(&(PVOID&)oPresent, HookedPresent);
        DetourTransactionCommit();
    }
}

static LRESULT WINAPI HookedPresent(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags) {
    static GraphicsAPI graphics;
    static bool inited = false;
    if (!inited) {
         inited = true;
         ID3D10Device* pDevice10;
         ID3D11Device* pDevice11;
         ID3D12Device* pDevice12;
         DetourTransactionBegin();
         if (pSwapChain->GetDevice(__uuidof(pDevice10), (void**)&pDevice10) == S_OK) {
             LOG_INFO << "GAME USES DIRECTX10" << LOG_FLUSH;
             graphics = GraphicsAPI::D3D10;
             DetourAttach(&(PVOID&)oResizeBuffers, dx10::HookedResizeBuffers);
         }
         else if (pSwapChain->GetDevice(__uuidof(pDevice11), (void**)&pDevice11) == S_OK) {
             LOG_INFO << "GAME USES DIRECTX11" << LOG_FLUSH;
             graphics = GraphicsAPI::D3D11;
             DetourAttach(&(PVOID&)oResizeBuffers, dx11::HookedResizeBuffers);
         }
         else if (pSwapChain->GetDevice(__uuidof(pDevice12), (void**)&pDevice12) == S_OK) {
             LOG_INFO << "GAME USES DIRECTX12" << LOG_FLUSH;
             graphics = GraphicsAPI::D3D12;
             DetourAttach(&(PVOID&)oResizeBuffers, dx12::HookedResizeBuffers);
             DetourAttach(&(PVOID&)dx12::oExecuteCommandLists, dx12::HookedExecuteCommandLists);
         }
         DetourTransactionCommit();
    }

    switch (graphics) {
    case GraphicsAPI::D3D10:
        dx10::Render(pSwapChain);
        break;
    case GraphicsAPI::D3D11:
        dx11::Render(pSwapChain);
        break;
    case GraphicsAPI::D3D12:
        dx12::Render((dx12::IDXGISwapChain3*)pSwapChain);
        break;
    }

    return oPresent(pSwapChain, syncInterval, flags);
}

static void InitImGuiContext(const ImGuiContextParams& params) {
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
        params.pSwapChain->GetDevice(IID_PPV_ARGS(&dx10::g_pDevice));
        params.pSwapChain->GetDesc(&swapChainDesc);
        hwnd = swapChainDesc.OutputWindow;
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX10_Init(dx10::g_pDevice);
    }
    if (params.api == GraphicsAPI::D3D11) {
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        params.pSwapChain->GetDevice(IID_PPV_ARGS(&dx11::g_pDevice));
        params.pSwapChain->GetDesc(&swapChainDesc);
        dx11::g_pDevice->GetImmediateContext(&dx11::g_pDeviceContext);
        hwnd = swapChainDesc.OutputWindow;
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(dx11::g_pDevice, dx11::g_pDeviceContext);
    }
    if (params.api == GraphicsAPI::D3D12) {
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        params.pSwapChain->GetDesc(&swapChainDesc);
        hwnd = swapChainDesc.OutputWindow;
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);
    }

    if (g_details.ApplyStyleProc) {
        g_details.ApplyStyleProc();
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
        case WM_KEYUP: if (wParam != VK_OEM_3) break;
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

namespace opengl {
    static void Hook() {
        HMODULE module = GetModuleHandleA("opengl32.dll");

        oSwapBuffers = (swapBuffers_t)GetProcAddress(module, "wglSwapBuffers");
        if (oSwapBuffers == 0) {
            LOG_ERROR << "[OPENGL] Unable to find wglSwapBuffers. Hook not installed" << LOG_FLUSH;
            return;
        }
        LOG_DEBUG << "[OPENGL] wglSwapBuffers address is " << oSwapBuffers << LOG_FLUSH;
        DetourTransactionBegin();
        DetourAttach(&(PVOID&)oSwapBuffers, HookedSwapBuffers);
        DetourTransactionCommit();
    }

    static bool WINAPI HookedSwapBuffers(HDC hdc) {
        if (!ImGui::GetCurrentContext()) {
            ImGuiContextParams params = { GraphicsAPI::OpenGL, nullptr, hdc, nullptr };
            InitImGuiContext(params);
            LOG_INFO << "GAME USES OPENGL" << LOG_FLUSH;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        if (g_visible) {
            g_details.DrawMenuProc(nullptr);
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        return oSwapBuffers(hdc);
    }
} // opengl

namespace dx9 {
    static void Hook() {
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
        d3dpp.hDeviceWindow = g_dummyHWND;
        HRESULT result = d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &dummyDev);
        if (result != D3D_OK) {
            LOG_ERROR << "[D3D9] IDirect3D9::CreateDevice returned " << result << ". Hook not installed" << LOG_FLUSH;
            d3d9->Release();
            return;
        }

        void** pVTable = *(void***)(dummyDev);
        oEndScene = (endScene_t)pVTable[42];
        oReset = (reset_t)pVTable[16];

        LOG_DEBUG << "[D3D9] IDirect3DDevice9::EndScene address is " << oEndScene << LOG_FLUSH;
        LOG_DEBUG << "[D3D9] IDirect3DDevice9::Reset address is " << oReset << LOG_FLUSH;

        dummyDev->Release();
        d3d9->Release();

        DetourTransactionBegin();
        DetourAttach(&(PVOID&)oEndScene, HookedEndScene);
        DetourAttach(&(PVOID&)oReset, HookedReset);
        DetourTransactionCommit();
    }

    static HRESULT WINAPI HookedEndScene(LPDIRECT3DDEVICE9 pDevice) {
        static bool inited = false;
        if (!inited) {
            inited = true;
            ImGuiContextParams params = { GraphicsAPI::D3D9, pDevice, 0, nullptr };
            InitImGuiContext(params);
            LOG_INFO << "GAME USES DIRECTX9" << LOG_FLUSH;
        }

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        if (g_visible) {
            g_details.DrawMenuProc(nullptr);
        }
        ImGui::EndFrame();

        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

        return oEndScene(pDevice);
    }

    static HRESULT WINAPI HookedReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
        ImGui_ImplDX9_InvalidateDeviceObjects();
        HRESULT result = oReset(pDevice, pPresentationParameters);
        ImGui_ImplDX9_CreateDeviceObjects();
        LOG_DEBUG << "[D3D9] Swap chain was reset" << LOG_FLUSH;
        return result;
    }
} // dx9

namespace dx10 {
    static void Setup() {
        if (oPresent && oResizeBuffers) {
            return;
        }

        HMODULE module = GetModuleHandle("d3d10.dll");

        DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
        swapChainDesc.Windowed = TRUE;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = g_dummyHWND;
        swapChainDesc.SampleDesc.Count = 1;

        IDXGISwapChain* pSwapChain;
        ID3D10Device* pDevice;
        HRESULT result = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_NULL, NULL, 0, D3D10_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice);
        if (result != S_OK) {
            LOG_ERROR << "[D3D10] D3D10CreateDeviceAndSwapChain returned " << result << ". Hook not installed" << LOG_FLUSH;
            return;
        }

        void** pVTable = *reinterpret_cast<void***>(pSwapChain);
        oPresent = (present_t)pVTable[8];
        oResizeBuffers = (resizeBuffers_t)pVTable[13];

        LOG_DEBUG << "[D3D10] Present address is " << oPresent << LOG_FLUSH;
        LOG_DEBUG << "[D3D10] ResizeBuffers address is " << oResizeBuffers << LOG_FLUSH;

        pSwapChain->Release();
        pDevice->Release();
    }

    static void Render(IDXGISwapChain* pSwapChain) {
        if (!ImGui::GetCurrentContext()) {
            ImGuiContextParams params;
            params.api = GraphicsAPI::D3D10;
            params.pSwapChain = pSwapChain;
            InitImGuiContext(params);
        }

        if (!g_pRenderTarget) {
            CreateRenderTarget(pSwapChain);
        }

        ImGui_ImplDX10_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        if (g_visible) {
            g_details.DrawMenuProc(nullptr);
        }
        ImGui::EndFrame();
        ImGui::Render();

        g_pDevice->OMSetRenderTargets(1, &g_pRenderTarget, nullptr);
        ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());
    }

    static HRESULT WINAPI HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width,
                                              UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
        if (g_pRenderTarget) {
            g_pRenderTarget->Release();
            g_pRenderTarget = nullptr;
        }
        LOG_DEBUG << "[D3D10] Buffers were resized" << LOG_FLUSH;
        return oResizeBuffers(pSwapChain, bufferCount, width, height, newFormat, swapChainFlags);
    }

    static void CreateRenderTarget(IDXGISwapChain* pSwapChain) {
        ID3D10Texture2D* pBackBuffer = NULL;
        pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        if (pBackBuffer) {
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            g_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTarget);
            pBackBuffer->Release();
        }
    }
} // dx10

namespace dx11 {
    static void Setup() {
        if (oPresent && oResizeBuffers) {
            return;
        }

        HMODULE module = GetModuleHandle("d3d11.dll");

        DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
        swapChainDesc.Windowed = TRUE;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = g_dummyHWND;
        swapChainDesc.SampleDesc.Count = 1;

        IDXGISwapChain* pSwapChain;
        ID3D11Device* pDevice;
        const D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_0
        };
        HRESULT result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_NULL, NULL, 0, featureLevels, 2, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, nullptr, &g_pDeviceContext);
        if (result != S_OK) {
            LOG_ERROR << "[D3D11] D3D11CreateDeviceAndSwapChain returned " << result << ". Hook not installed" << LOG_FLUSH;
            return;
        }

        void** pVTable = *(void***)(pSwapChain);
        oPresent = (present_t)pVTable[8];
        oResizeBuffers = (resizeBuffers_t)pVTable[13];

        pSwapChain->Release();
        pDevice->Release();

        LOG_DEBUG << "[D3D11] Present address is " << oPresent << LOG_FLUSH;
        LOG_DEBUG << "[D3D11] ResizeBuffers address is " << oResizeBuffers << LOG_FLUSH;
    }

    static void CreateRenderTarget(IDXGISwapChain* pSwapChain) {
        ID3D11Texture2D* pBackBuffer = NULL;
        pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        if (pBackBuffer && g_pDevice) {
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            g_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTarget);
            pBackBuffer->Release();
        }
    }

    static void Render(IDXGISwapChain* pSwapChain) {
        if (!ImGui::GetCurrentContext()) {
            ImGuiContextParams params;
            params.api = GraphicsAPI::D3D11;
            params.pSwapChain = pSwapChain;
            InitImGuiContext(params);
        }

        if (!g_pRenderTarget) {
            CreateRenderTarget(pSwapChain);
        }
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        if (g_visible) {
            g_details.DrawMenuProc(nullptr);
        }
        ImGui::EndFrame();
        ImGui::Render();
        g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTarget, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    static HRESULT WINAPI HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width,
                                              UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
        if (g_pRenderTarget) {
            g_pRenderTarget->Release();
            g_pRenderTarget = nullptr;
        }
        LOG_DEBUG << "[D3D11] Buffers were resized" << LOG_FLUSH;
        return oResizeBuffers(pSwapChain, bufferCount, width, height, newFormat, swapChainFlags);
    }
} // dx11

namespace dx12 {
    static void Setup() {
        DXGI_SWAP_CHAIN_DESC1 sd = { };
        sd.BufferCount = NUM_BACK_BUFFERS;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.SampleDesc.Count = 1;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        // Create device
        D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
        if (D3D12CreateDevice(NULL, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK) { return; }

        D3D12_COMMAND_QUEUE_DESC desc = { };
        if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK) { return; }

        IDXGISwapChain1* swapChain1 = NULL;
        if (CreateDXGIFactory1(IID_PPV_ARGS(&g_dxgiFactory)) != S_OK) { return; }
        if (g_dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, g_dummyHWND, &sd, NULL, NULL, &swapChain1) != S_OK) { return; }
        if (swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK) { return; }
        for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i) {
            if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_commandAllocators[i])) != S_OK) {
                LOG_DEBUG << "CreateCommandAllocator failed" << LOG_FLUSH;
                return;
            }
        }
        if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_commandAllocators[0], NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK) { 
            return; 
        }
        swapChain1->Release();

        // Hook
        void** pVTable = *reinterpret_cast<void***>(g_pSwapChain);
        void** pCommandQueueVTable = *reinterpret_cast<void***>(g_pd3dCommandQueue);

        oPresent = (present_t)pVTable[8];
        oResizeBuffers = (resizeBuffers_t)pVTable[13];
        oExecuteCommandLists = (executeCommandLists_t)pCommandQueueVTable[10];

        LOG_DEBUG << "[D3D12] oPresent address is " << oPresent << LOG_FLUSH;
        LOG_DEBUG << "[D3D12] oResizeBuffers is " << oResizeBuffers << LOG_FLUSH;
        LOG_DEBUG << "[D3D12] oExecuteCommandLists is " << oExecuteCommandLists << LOG_FLUSH;

        if (g_pd3dCommandQueue) {
            g_pd3dCommandQueue->Release();
            g_pd3dCommandQueue = NULL;
        }
        for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i) {
            if (g_mainRenderTargetResource[i]) {
                g_mainRenderTargetResource[i]->Release();
                g_mainRenderTargetResource[i] = NULL;
            }
        }

        if (g_pSwapChain) {
            g_pSwapChain->Release();
            g_pSwapChain = NULL;
        }
        for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i) {
            if (g_commandAllocators[i]) {
                g_commandAllocators[i]->Release();
                g_commandAllocators[i] = NULL;
            }
        }
        if (g_pd3dCommandList) {
            g_pd3dCommandList->Release();
            g_pd3dCommandList = NULL;
        }
        if (g_pd3dRtvDescHeap) {
            g_pd3dRtvDescHeap->Release();
            g_pd3dRtvDescHeap = NULL;
        }
        if (g_pd3dSrvDescHeap) {
            g_pd3dSrvDescHeap->Release();
            g_pd3dSrvDescHeap = NULL;
        }
        if (g_pd3dDevice) {
            g_pd3dDevice->Release();
            g_pd3dDevice = NULL;
        }
        if (g_dxgiFactory) {
            g_dxgiFactory->Release();
            g_dxgiFactory = NULL;
        }
    }

    static void Render(IDXGISwapChain3* pSwapChain) {
        if (!ImGui::GetCurrentContext()) {
            ImGuiContextParams params;
            params.api = GraphicsAPI::D3D12;
            params.pSwapChain = pSwapChain;
            InitImGuiContext(params);
        }

        if (!ImGui::GetIO().BackendRendererUserData) {
            if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&g_pd3dDevice)))) {
                {
                    D3D12_DESCRIPTOR_HEAP_DESC desc = { };
                    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                    desc.NumDescriptors = NUM_BACK_BUFFERS;
                    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                    desc.NodeMask = 1;
                    if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK) { return; }

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
                    if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK) { return; }
                }

                for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i) {
                    if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_commandAllocators[i])) != S_OK) {
                        return;
                    }
                }

                if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_commandAllocators[0], NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
                        g_pd3dCommandList->Close() != S_OK) {
                    return;
                }

                ImGui_ImplDX12_Init(g_pd3dDevice, NUM_BACK_BUFFERS,
                DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
                g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
                g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
            }
        }

        if (!g_mainRenderTargetResource[0]) {
            CreateRenderTarget(pSwapChain);
        }

        if (ImGui::GetCurrentContext() && g_pd3dCommandQueue && g_mainRenderTargetResource[0]) {
            ImGui_ImplDX12_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            if (g_visible) {
                g_details.DrawMenuProc(nullptr);
            }

            ImGui::Render();

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
            g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL);
            g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
            ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
            g_pd3dCommandList->ResourceBarrier(1, &barrier);
            g_pd3dCommandList->Close();

            g_pd3dCommandQueue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&g_pd3dCommandList));
        }
    }

    static HRESULT WINAPI HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, 
                                              DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
        for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i) {
            if (g_mainRenderTargetResource[i]) {
                g_mainRenderTargetResource[i]->Release();
                g_mainRenderTargetResource[i] = NULL;
            }
        }
         return oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    }

    static void WINAPI HookedExecuteCommandLists(ID3D12CommandQueue* pCommandQueue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists) {
        if (!g_pd3dCommandQueue) {
            g_pd3dCommandQueue = pCommandQueue;
        }
        return oExecuteCommandLists(pCommandQueue, NumCommandLists, ppCommandLists);
    }

    static void CreateRenderTarget(IDXGISwapChain* pSwapChain) {
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
} // dx12