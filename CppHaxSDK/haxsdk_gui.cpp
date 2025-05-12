#include "haxsdk_gui.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <TlHelp32.h>
#include <fstream>
#include <format>
#include <algorithm>

// opengl
#pragma comment(lib, "OpenGL32.lib")

// directx 9
#include <d3d9.h>
#pragma comment(lib, "D3d9.lib")

//directx 10
#include <d3d10_1.h>
#include <dxgi.h>
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "dxgi.lib")

//directx 11
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

// directx 12
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

// vulkan
#include <vulkan/vulkan.h>
#pragma comment(lib, "vulkan-1.lib")

#ifdef _WIN64
    #if __has_include("third_party/detours/x64/detours.h")
        #include "third_party/detours/x64/detours.h"
        #pragma comment(lib, "third_party/detours/x64/detours.lib")
    #else
        #include "../third_party/detours/x64/detours.h"
        #pragma comment(lib, "../third_party/detours/x64/detours.lib")
    #endif
#else
    #if __has_include("third_party/detours/x86/detours.h")
        #include "third_party/detours/x86/detours.h"
        #pragma comment(lib, "third_party/detours/x86/detours.lib")
    #else
        #include "../third_party/detours/x86/detours.h"
        #pragma comment(lib, "../third_party/detours/x86/detours.lib")
    #endif
#endif

#include "third_party/imgui/imgui.h"
#include "third_party/imgui/imgui_internal.h"
#include "third_party/imgui/backend/imgui_impl_dx9.h"
#include "third_party/imgui/backend/imgui_impl_dx10.h"
#include "third_party/imgui/backend/imgui_impl_dx11.h"
#include "third_party/imgui/backend/imgui_impl_dx12.h"
#include "third_party/imgui/backend/imgui_impl_vulkan.h"
#include "third_party/imgui/backend/imgui_impl_win32.h"
#include "third_party/imgui/backend/imgui_impl_opengl3.h"
#include "third_party/imgui/backend/imgui_impl_opengl3_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"

//#include "haxsdk.h"
#include "system/haxsdk_system.h"
#include "haxsdk_logger.h"

using setCursorPos_t            = BOOL(WINAPI*)(int, int);
using clipCursor_t              = BOOL(WINAPI*)(const RECT*);
using setPhysicalCursorPos_t    = BOOL(WINAPI*)(int, int);
using mouse_event_t             = void(WINAPI*)(DWORD, DWORD, DWORD, DWORD, ULONG_PTR);
using GetCursorPos_t            = BOOL(WINAPI*)(LPPOINT);
using SendInput_t               = UINT(WINAPI*)(UINT, LPINPUT, int);
using SendMessageW_t            = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
using vkAcquireNextImageKHR_t   = VkResult(VKAPI_CALL*)(VkDevice, VkSwapchainKHR, uint64_t, VkSemaphore, VkFence, uint32_t*);
using vkAcquireNextImage2KHR_t  = VkResult(VKAPI_CALL*)(VkDevice, const VkAcquireNextImageInfoKHR*, uint32_t*);
using vkQueuePresentKHR_t       = VkResult(VKAPI_CALL*)(VkQueue, const VkPresentInfoKHR*);
using vkCreateSwapchainKHR_t    = VkResult(VKAPI_CALL*)(VkDevice, const VkSwapchainCreateInfoKHR*, const VkAllocationCallbacks*, VkSwapchainKHR*);

using swapBuffers_t             = bool(WINAPI*)(HDC);
using reset_t                   = HRESULT(WINAPI*)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
using endScene_t                = HRESULT(WINAPI*)(LPDIRECT3DDEVICE9);
using present_t                 = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT);
using resizeBuffers_t           = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
using setRenderTargets_t        = void(WINAPI*)(ID3D10Device*, UINT, ID3D10RenderTargetView* const*, ID3D10DepthStencilView*);
using setRenderTargets11_t      = void(WINAPI*)(ID3D11DeviceContext*, UINT, ID3D11RenderTargetView* const*, ID3D11DepthStencilView*);
using executeCommandLists_t     = void(WINAPI*)(ID3D12CommandQueue*, UINT, ID3D12CommandList*);
using setRenderTargets12_t      = void(WINAPI*)(ID3D12GraphicsCommandList*, UINT, const D3D12_CPU_DESCRIPTOR_HANDLE*, BOOL, const D3D12_CPU_DESCRIPTOR_HANDLE*);

static GraphicsApi                      g_graphicsApi;
static HWND                             g_dummyHWND;
static WNDPROC                          oWndproc;
static setCursorPos_t                   oSetCursorPos;
static clipCursor_t                     oClipCursor;
static setPhysicalCursorPos_t           oSetPhysicalCursorPos;
static mouse_event_t                    oMouseEvent;
static GetCursorPos_t                   oGetCursorPos;
static SendInput_t                      oSendInput;
static SendMessageW_t                   oSendMessageW;
static swapBuffers_t                    oSwapBuffers;
static present_t                        oPresent;
static resizeBuffers_t                  oResizeBuffers;
static vkAcquireNextImageKHR_t          oVkAcquireNextImageKHR;
static vkAcquireNextImage2KHR_t         oVkAcquireNextImage2KHR;
static vkQueuePresentKHR_t              oVkQueuePresentKHR;
static vkCreateSwapchainKHR_t           oVkCreateSwapchainKHR;
static bool                             g_MenuVisible = true;
static int                              g_MenuHotkey = 0xC0;
static HWND                             g_GameHwnd;
static HANDLE                           g_CheatHandle;
static std::function<void()>            g_FnMenuRender;
static std::function<void()>            g_FnBackground;
static std::function<void()>            g_FnInitialize;

static HANDLE                           hRenderSemaphore;
constexpr DWORD                         MAX_RENDER_THREAD_COUNT = 5;

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
namespace vulkan
{
    static VkAllocationCallbacks* g_Allocator = NULL;
    static VkInstance g_Instance = VK_NULL_HANDLE;
    static VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
    static VkDevice g_FakeDevice = VK_NULL_HANDLE, g_Device = VK_NULL_HANDLE;

    static uint32_t g_QueueFamily = (uint32_t)-1;
    static std::vector<VkQueueFamilyProperties> g_QueueFamilies;

    static VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
    static VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
    static uint32_t g_MinImageCount = 2;
    static VkRenderPass g_RenderPass = VK_NULL_HANDLE;
    static ImGui_ImplVulkanH_Frame g_Frames[8] = { };
    static ImGui_ImplVulkanH_FrameSemaphores g_FrameSemaphores[8] = { };
    static VkExtent2D g_ImageExtent = {};
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct ImGuiContextParams {
    GraphicsApi                         graphicsApi;
    LPDIRECT3DDEVICE9                   pDevice9;
    HDC                                 hdc;
    IDXGISwapChain*                     pSwapChain;
};

static LRESULT WINAPI                   HookedPresent(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);
static LRESULT WINAPI                   HookedWndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static BOOL WINAPI                      HookedSetCursorPos(int X, int Y);
static BOOL WINAPI                      HookedClipCursor(const RECT* lpRect);
static BOOL WINAPI                      HookedSetPhysicalCursorPos(int x, int y);
static UINT WINAPI                      HookedSendInput(UINT cInputs, LPINPUT pInputs, int cbSize);
static void WINAPI                      HookedMouseEvent(DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, ULONG_PTR dwExtraInfo);
static LRESULT WINAPI                   HookedSendMessageW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

static void                             InitImGuiContext(const ImGuiContextParams& params);

namespace opengl {
    static void                         Hook();
    static bool WINAPI                  HookedSwapBuffers(HDC hdc);
    static HaxTexture                   LoadTextureFromResource(int32_t id);
}
namespace dx9 {
    static void                         Hook();
    static HRESULT WINAPI               HookedReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
    static HRESULT WINAPI               HookedEndScene(LPDIRECT3DDEVICE9 pDevice);
}
namespace dx10 {
    static void                         Setup();
    static void                         Render(IDXGISwapChain* pSwapChain);
    static void                         CreateRenderTarget(IDXGISwapChain* pSwapChain);
    static HRESULT WINAPI               HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
}
namespace dx11 {
    static void                         Setup();
    static void                         Render(IDXGISwapChain* pSwapChain);
    static void                         CreateRenderTarget(IDXGISwapChain* pSwapChain);
    static HRESULT WINAPI               HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
    static HaxTexture                   LoadTextureFromResource(int32_t id);
}
namespace dx12 {
    static void                         Setup();
    static void                         Render(IDXGISwapChain3* pSwapChain);
    static void                         CreateRenderTarget(IDXGISwapChain* pSwapChain);
    static HRESULT WINAPI               HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width,
                                                            UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    static void WINAPI                  HookedExecuteCommandLists(ID3D12CommandQueue* pCommandQueue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists);
}
namespace vulkan
{
    static void                         Setup();
    static bool                         CreateDevice();
    static void                         CleanupRenderTarget();
    static void                         CreateRenderTarget(VkDevice device, VkSwapchainKHR swapchain);
    static bool                         DoesQueueSupportGraphic(VkQueue queue, VkQueue* pGraphicQueue);
    static void                         RenderImGui_Vulkan(VkQueue queue, const VkPresentInfoKHR* pPresentInfo);
}

void HaxSdk::ImplementImGui(GraphicsApi graphics, std::function<void()> fnInit, std::function<void()> fnBackground, std::function<void()> fnRender) {
    g_graphicsApi = graphics;
    g_FnInitialize = fnInit;
    g_FnBackground = fnBackground;
    g_FnMenuRender = fnRender;

    WNDENUMPROC enumProc = [](HWND hwnd, LPARAM lParam) -> BOOL
    {
        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);
        
        if (pid != GetCurrentProcessId())
            return TRUE;

        wchar_t buff[64]{0};
        GetClassNameW(hwnd, buff, sizeof(buff) / sizeof(wchar_t));
        if (wcscmp(buff, L"UnityWndClass") == 0)
        {
            g_GameHwnd = hwnd;
            return FALSE;
        }
        return TRUE;
    };
    EnumWindows(enumProc, 0);

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
    }

    DWORD processId = GetProcessId(GetCurrentProcess());
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);

    MODULEENTRY32 me{};
    me.dwSize = sizeof(MODULEENTRY32);
    if (Module32First(snapshot, &me)) {
        do {
            std::string moduleName(me.szModule);
            std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), ::tolower);
            if (moduleName == "opengl32.dll" && (g_graphicsApi & GraphicsApi_OpenGL)) {
                opengl::Hook();
            }
            if (moduleName == "d3d9.dll" && (g_graphicsApi & GraphicsApi_DirectX9)) {
                dx9::Hook();
            }
            if (moduleName == "d3d10.dll" && (g_graphicsApi & GraphicsApi_DirectX10)) {
                dx10::Setup();
            }
            if (moduleName == "d3d11.dll" && (g_graphicsApi & GraphicsApi_DirectX11)) {
                dx11::Setup();
            }
            if (moduleName == "d3d12.dll" && (g_graphicsApi & GraphicsApi_DirectX12)) {
                dx12::Setup();
            }
            if (moduleName == "vulkan-1.dll" && (g_graphicsApi & GraphicsApi_Vulkan)) {
                vulkan::Setup();
            }
        } while (Module32Next(snapshot, &me));
    }
    CloseHandle(snapshot);

    if (dummyWindow.cbSize > 0) {
        DestroyWindow(g_dummyHWND);
        UnregisterClass(dummyWindow.lpszClassName, dummyWindow.hInstance);
    }

    if (oPresent) {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)oPresent, HookedPresent);
        DetourTransactionCommit();
    }
}

void HaxSdk::AddText(ImFont* font, const char* text, const ImVec2& pos, ImU32 col, float fontSize, TextShift shift)
{
    ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text);
    ImDrawList* pDrawList = ImGui::GetBackgroundDrawList();
    float xShift = shift == TextShift_Right ? 0.f : textSize.x / (float)shift;
    pDrawList->AddText(font, fontSize, ImVec2(pos.x - xShift + 1.F, pos.y - 1.F), IM_COL32_BLACK, text);
    pDrawList->AddText(font, fontSize, ImVec2(pos.x - xShift + 1.F, pos.y + 1.F), IM_COL32_BLACK, text);
    pDrawList->AddText(font, fontSize, ImVec2(pos.x - xShift - 1.F, pos.y + 1.F), IM_COL32_BLACK, text);
    pDrawList->AddText(font, fontSize, ImVec2(pos.x - xShift - 1.F, pos.y - 1.F), IM_COL32_BLACK, text);
    pDrawList->AddText(font, fontSize, ImVec2(pos.x - xShift, pos.y), col, text);
}

void HaxSdk::Shutdown() {
    
}

HaxTexture HaxSdk::LoadTextureFromResource(int32_t id) {
    if (g_graphicsApi == GraphicsApi_DirectX11)
        return dx11::LoadTextureFromResource(id);
    if (g_graphicsApi == GraphicsApi_OpenGL)
        return opengl::LoadTextureFromResource(id);
    assert(false && "Not implemented");
    return HaxTexture();
}

void HaxSdk::SetCheatHandle(HANDLE handle)
{
    g_CheatHandle = handle;
}

static LRESULT WINAPI HookedPresent(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags) {
    static bool inited = false;
    if (!inited) {
         inited = true;
         ID3D10Device* pDevice10;
         ID3D11Device* pDevice11;
         ID3D12Device* pDevice12;
         DetourTransactionBegin();
         DetourUpdateThread(GetCurrentThread());
         if ((g_graphicsApi & GraphicsApi_DirectX10) && pSwapChain->GetDevice(__uuidof(pDevice10), (void**)&pDevice10) == S_OK) {
             g_graphicsApi = GraphicsApi_DirectX10;
             HAX_LOG("GAME USES DIRECTX10");
             ::DetourAttach(&(PVOID&)oResizeBuffers, dx10::HookedResizeBuffers);
         }
         else if ((g_graphicsApi & GraphicsApi_DirectX11) && pSwapChain->GetDevice(__uuidof(pDevice11), (void**)&pDevice11) == S_OK) {
             g_graphicsApi = GraphicsApi_DirectX11;
             HAX_LOG("GAME USES DIRECTX11");
             ::DetourAttach(&(PVOID&)oResizeBuffers, dx11::HookedResizeBuffers);
         }
         else if ((g_graphicsApi & GraphicsApi_DirectX12) && pSwapChain->GetDevice(__uuidof(pDevice12), (void**)&pDevice12) == S_OK) {
             g_graphicsApi = GraphicsApi_DirectX12;
             HAX_LOG("GAME USES DIRECTX12");
             ::DetourAttach(&(PVOID&)oResizeBuffers, dx12::HookedResizeBuffers);
             ::DetourAttach(&(PVOID&)dx12::oExecuteCommandLists, dx12::HookedExecuteCommandLists);
         }
         else {
             g_graphicsApi = GraphicsApi_None;
         }
         DetourTransactionCommit();
    }

    WaitForSingleObject(hRenderSemaphore, INFINITE);
    if (g_graphicsApi & GraphicsApi_DirectX10) {
        dx10::Render(pSwapChain);
    }
    else if (g_graphicsApi & GraphicsApi_DirectX11) {
        dx11::Render(pSwapChain);
    }
    else if (g_graphicsApi & GraphicsApi_DirectX12) {
        dx12::Render((dx12::IDXGISwapChain3*)pSwapChain);
    }

    HRESULT result = oPresent(pSwapChain, syncInterval, flags);
    ReleaseSemaphore(hRenderSemaphore, 1, NULL);
    return result;
}

static void InitImGuiContext(const ImGuiContextParams& params) {
    //HWND hwnd = 0;
    if (params.graphicsApi & GraphicsApi_OpenGL) {
        //hwnd = WindowFromDC(params.hdc);
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(g_GameHwnd);
        ImGui_ImplOpenGL3_Init();
    }
    if (params.graphicsApi & GraphicsApi_DirectX9) {
        D3DDEVICE_CREATION_PARAMETERS creationParams;
        params.pDevice9->GetCreationParameters(&creationParams);
        //hwnd = creationParams.hFocusWindow;
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(g_GameHwnd);
        ImGui_ImplDX9_Init(params.pDevice9);
    }
    if (params.graphicsApi & GraphicsApi_DirectX10) {
        /*DXGI_SWAP_CHAIN_DESC swapChainDesc;
        params.pSwapChain->GetDevice(IID_PPV_ARGS(&dx10::g_pDevice));
        params.pSwapChain->GetDesc(&swapChainDesc);
        hwnd = swapChainDesc.OutputWindow;*/
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(g_GameHwnd);
        ImGui_ImplDX10_Init(dx10::g_pDevice);
    }
    if (params.graphicsApi & GraphicsApi_DirectX11) {
        //DXGI_SWAP_CHAIN_DESC swapChainDesc;
        params.pSwapChain->GetDevice(IID_PPV_ARGS(&dx11::g_pDevice));
        //params.pSwapChain->GetDesc(&swapChainDesc);
        dx11::g_pDevice->GetImmediateContext(&dx11::g_pDeviceContext);
        //hwnd = swapChainDesc.OutputWindow;
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(g_GameHwnd);
        ImGui_ImplDX11_Init(dx11::g_pDevice, dx11::g_pDeviceContext);
    }
    if (params.graphicsApi & GraphicsApi_DirectX12) {
        /*DXGI_SWAP_CHAIN_DESC swapChainDesc;
        params.pSwapChain->GetDesc(&swapChainDesc);
        hwnd = swapChainDesc.OutputWindow;*/
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(g_GameHwnd);
    }
    if (params.graphicsApi & GraphicsApi_Vulkan)
    {
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(g_GameHwnd);
    }
    unsafe::Thread::Attach();

    ImGui::GetIO().IniFilename = nullptr;
    if (g_FnInitialize)
        g_FnInitialize();

    ImGuiIO& io = ImGui::GetIO();
    io.WantCaptureMouse = true;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
    oWndproc = (WNDPROC)SetWindowLongPtr(g_GameHwnd, GWLP_WNDPROC, (LONG_PTR)HookedWndproc);

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    if (HMODULE hModule = GetModuleHandleA("user32.dll")) {
        if (oClipCursor = (clipCursor_t)GetProcAddress(hModule, "ClipCursor"))
            ::DetourAttach(&(PVOID&)oClipCursor, HookedClipCursor);
        else
            HAX_LOG_ERROR("Unable to hook ClipCursor");

        if (oSetPhysicalCursorPos = (setPhysicalCursorPos_t)GetProcAddress(hModule, "SetPhysicalCursorPos"))
            ::DetourAttach(&(PVOID&)oSetPhysicalCursorPos, HookedSetPhysicalCursorPos);
        else
            HAX_LOG_ERROR("Unable to hook SetPhysicalCursorPos");

        if (oSetCursorPos = (setCursorPos_t)GetProcAddress(hModule, "SetCursorPos"))
            ::DetourAttach(&(PVOID&)oSetCursorPos, HookedSetCursorPos);
        else
            HAX_LOG_ERROR("Unable to hook SetCursorPos");

        if (oMouseEvent = (mouse_event_t)GetProcAddress(hModule, "mouse_event"))
            ::DetourAttach(&(PVOID&)oMouseEvent, HookedMouseEvent);
        else
            HAX_LOG_ERROR("Unable to hook mouse_event");

        if (oSendInput = (SendInput_t)GetProcAddress(hModule, "SendInput"))
            ::DetourAttach(&(PVOID&)oSendInput, HookedSendInput);
        else
            HAX_LOG_ERROR("Unable to hook SendInput");

        if (oSendMessageW = (SendMessageW_t)GetProcAddress(hModule, "SendMessageW"))
            ::DetourAttach(&(PVOID&)oSendMessageW, HookedSendMessageW);
        else
            HAX_LOG_ERROR("Unable to hook SendMessageW");
    }
    DetourTransactionCommit();

    hRenderSemaphore = CreateSemaphore(
        NULL,                                 // default security attributes
        MAX_RENDER_THREAD_COUNT,              // initial count
        MAX_RENDER_THREAD_COUNT,              // maximum count
        NULL                                  // unnamed semaphore);
    );
}

static LRESULT WINAPI HookedWndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    ImGuiIO& io = ImGui::GetIO();
    POINT position;
    GetCursorPos(&position);
    ScreenToClient(hWnd, &position);
    io.MousePos.x = (float)position.x;
    io.MousePos.y = (float)position.y;

    //auto& globals = HaxSdk::GetGlobals();
    if (uMsg == WM_KEYUP && wParam == g_MenuHotkey) {
        g_MenuVisible = !g_MenuVisible;
        io.MouseDrawCursor = g_MenuVisible;
    }

    if (uMsg == WM_CLOSE) {
        HAX_LOG_DEBUG("Got WM_QUIT message. Cheat should shut down");
        //HaxSdk::GetGlobals().ShouldExit = true;
    }

    if (g_MenuVisible) {
        RECT rect;
        GetWindowRect(hWnd, &rect);
        HookedClipCursor(&rect);

        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
        switch (uMsg) {
        case WM_KEYUP: 
            if (wParam != g_MenuHotkey) break;
        case WM_MOUSEMOVE:
        case WM_MOUSEACTIVATE:
        case WM_MOUSEHOVER:
        case WM_NCHITTEST:
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
    return CallWindowProc(oWndproc, hWnd, uMsg, wParam, lParam);
}

static BOOL WINAPI HookedSetCursorPos(int X, int Y) {
    return g_MenuVisible ? true : oSetCursorPos(X, Y);
}

static BOOL WINAPI HookedClipCursor(const RECT* lpRect) {
    return oClipCursor(g_MenuVisible ? NULL : lpRect);
}

static BOOL WINAPI HookedSetPhysicalCursorPos(int x, int y) {
    return g_MenuVisible ? true : oSetPhysicalCursorPos(x, y);
}

static UINT WINAPI HookedSendInput(UINT cInputs, LPINPUT pInputs, int cbSize) {
    return g_MenuVisible ? TRUE : oSendInput(cInputs, pInputs, cbSize);
}

static void WINAPI HookedMouseEvent(DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, ULONG_PTR dwExtraInfo) {
    if (!g_MenuVisible) { oMouseEvent(dwFlags, dx, dy, dwData, dwExtraInfo); }
}

static LRESULT WINAPI HookedSendMessageW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    return (g_MenuVisible && Msg == 0x20) ? TRUE : oSendMessageW(hWnd, Msg, wParam, lParam);
}

namespace opengl {
    static void Hook() {
        HMODULE module = GetModuleHandleA("opengl32.dll");

        oSwapBuffers = (swapBuffers_t)GetProcAddress(module, "wglSwapBuffers");
        if (oSwapBuffers == 0) {
            HAX_LOG_ERROR("[OPENGL] Unable to find wglSwapBuffers.Hook not installed");
            return;
        }
        HAX_LOG_DEBUG("[OPENGL] wglSwapBuffers address is {}", (void*)oSwapBuffers);
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)oSwapBuffers, HookedSwapBuffers);
        DetourTransactionCommit();
    }

    static bool WINAPI HookedSwapBuffers(HDC hdc) {
        if (!ImGui::GetCurrentContext()) {
            ImGuiContextParams params = { GraphicsApi_OpenGL, nullptr, hdc, nullptr };
            InitImGuiContext(params);
            HAX_LOG("GAME USES OPENGL");
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        try
        {
            if (g_FnBackground)
                g_FnBackground();

            if (g_MenuVisible && g_FnMenuRender)
                g_FnMenuRender();
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            HaxSdk::LogError(message ? message.UTF8() : "Exception without message");
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        return oSwapBuffers(hdc);
    }

    static HaxTexture LoadTextureFromResource(int32_t id) {
        LPVOID pointerToResource = nullptr;
        DWORD sizeOfResource;
        HMODULE hCheatModule = (HMODULE)g_CheatHandle;
        HRSRC hResInfo = FindResourceW(hCheatModule, MAKEINTRESOURCEW(id), L"PNG");
        if (hResInfo) {
            HGLOBAL hResData = LoadResource(hCheatModule, hResInfo);
            if (hResData) {
                pointerToResource = LockResource(hResData);
                sizeOfResource = SizeofResource(hCheatModule, hResInfo);
            }
        }
        assert(pointerToResource && "Invalid image");
        HaxTexture result{};
        int width, height;
        auto image_data = stbi_load_from_memory((stbi_uc*)pointerToResource, sizeOfResource, &width, &height, NULL, 4);
        result.m_height = static_cast<float>(height);
        result.m_width = static_cast<float>(width);

        // Create a OpenGL texture identifier
        GLuint image_texture;
        glGenTextures(1, &image_texture);
        glBindTexture(GL_TEXTURE_2D, image_texture);

        // Setup filtering parameters for display
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload pixels into texture
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<int32_t>(result.m_width), static_cast<int32_t>(result.m_height), 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        stbi_image_free(image_data);
        result.m_pTexture = (void*)(intptr_t)image_texture;

        return result;
    }
} // opengl

namespace dx9 {
    static void Hook() {
        HMODULE module = ::GetModuleHandle("d3d9.dll");

        LPDIRECT3D9 d3d9 = ::Direct3DCreate9(D3D_SDK_VERSION);
        if (d3d9 == NULL) {
            HAX_LOG_ERROR("[D3D9] Direct3DCreate9 failed. Hook not installed");
            return;
        }
        LPDIRECT3DDEVICE9 dummyDev = NULL;

        D3DPRESENT_PARAMETERS d3dpp = {};
        d3dpp.Windowed = false;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow = g_dummyHWND;
        HRESULT result = d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &dummyDev);
        if (result != D3D_OK) {
            HAX_LOG_ERROR("[D3D9] IDirect3D9::CreateDevice returned {}. Hook not installed", result);
            d3d9->Release();
            return;
        }

        void** pVTable = *(void***)(dummyDev);
        oEndScene = (endScene_t)pVTable[42];
        oReset = (reset_t)pVTable[16];

        HAX_LOG_DEBUG("[D3D9] IDirect3DDevice9::EndScene address is {}", (void*)oEndScene);
        HAX_LOG_DEBUG("[D3D9] IDirect3DDevice9::Reset address is {}", (void*)oReset);

        dummyDev->Release();
        d3d9->Release();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        ::DetourAttach(&(PVOID&)oEndScene, HookedEndScene);
        ::DetourAttach(&(PVOID&)oReset, HookedReset);
        DetourTransactionCommit();
    }

    static HRESULT WINAPI HookedEndScene(LPDIRECT3DDEVICE9 pDevice) {
        static bool inited = false;
        if (!inited) {
            inited = true;
            ImGuiContextParams params = { GraphicsApi_DirectX9, pDevice, 0, nullptr };
            InitImGuiContext(params);
            HAX_LOG("GAME USES DIRECTX9");
        }

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        try
        {
            if (g_FnBackground)
                g_FnBackground();

            if (g_MenuVisible && g_FnMenuRender)
                g_FnMenuRender();
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            HaxSdk::LogError(message ? message.UTF8() : "Exception without message");
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
            HAX_LOG_ERROR("[D3D10] D3D10CreateDeviceAndSwapChain returned {}. Hook not installed", result);
            return;
        }

        void** pVTable = *reinterpret_cast<void***>(pSwapChain);
        oPresent = (present_t)pVTable[8];
        oResizeBuffers = (resizeBuffers_t)pVTable[13];

        HAX_LOG_DEBUG("[D3D10] Present address is {}", (void*)oPresent);
        HAX_LOG_DEBUG("[D3D10] ResizeBuffers address is {}", (void*)oResizeBuffers);

        pSwapChain->Release();
        pDevice->Release();
    }

    static void Render(IDXGISwapChain* pSwapChain) {
        if (!ImGui::GetCurrentContext()) {
            ImGuiContextParams params;
            params.graphicsApi = GraphicsApi_DirectX10;
            params.pSwapChain = pSwapChain;
            InitImGuiContext(params);
        }

        if (!g_pRenderTarget) {
            CreateRenderTarget(pSwapChain);
        }

        ImGui_ImplDX10_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        try
        {
            if (g_FnBackground)
                g_FnBackground();

            if (g_MenuVisible && g_FnMenuRender)
                g_FnMenuRender();
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            HaxSdk::LogError(message ? message.UTF8() : "Exception without message");
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

        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
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
            HAX_LOG_ERROR("[D3D11] D3D11CreateDeviceAndSwapChain returned {}. Hook not installed", result);
            return;
        }

        void** pVTable = *(void***)(pSwapChain);
        oPresent = (present_t)pVTable[8];
        oResizeBuffers = (resizeBuffers_t)pVTable[13];

        pSwapChain->Release();
        pDevice->Release();

        HAX_LOG_DEBUG("[D3D11] Present address is {}", (void*)oPresent);
        HAX_LOG_DEBUG("[D3D11] ResizeBuffers address is {}", (void*)oResizeBuffers);
    }

    static void CreateRenderTarget(IDXGISwapChain* pSwapChain) {
        ID3D11Texture2D* pBackBuffer = NULL;
        pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        if (pBackBuffer && g_pDevice) {
            D3D11_RENDER_TARGET_VIEW_DESC desc = {}; // to fix menu brightness
            memset(&desc, 0, sizeof(desc));
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            g_pDevice->CreateRenderTargetView(pBackBuffer, &desc, &g_pRenderTarget);
            pBackBuffer->Release();
        }
    }

    static void Render(IDXGISwapChain* pSwapChain) {
        if (!ImGui::GetCurrentContext()) {
            ImGuiContextParams params;
            params.graphicsApi = GraphicsApi_DirectX11;
            params.pSwapChain = pSwapChain;
            InitImGuiContext(params);
        }

        if (!g_pRenderTarget) {
            CreateRenderTarget(pSwapChain);
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        try
        {
            if (g_FnBackground)
                g_FnBackground();

            if (g_MenuVisible && g_FnMenuRender)
                g_FnMenuRender();
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            HaxSdk::LogError(message ? message.UTF8() : "Exception without message");
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
        return oResizeBuffers(pSwapChain, bufferCount, width, height, newFormat, swapChainFlags);
    }

    static HaxTexture LoadTextureFromResource(int32_t id) {
        LPVOID pointerToResource = nullptr;
        DWORD sizeOfResource;
        HMODULE hCheatModule = (HMODULE)g_CheatHandle;
        if (HRSRC hResInfo = FindResourceW(hCheatModule, MAKEINTRESOURCEW(id), L"PNG")) {
            if (HGLOBAL hResData = LoadResource(hCheatModule, hResInfo)) {
                pointerToResource = LockResource(hResData);
                sizeOfResource = SizeofResource(hCheatModule, hResInfo);
            }
        }
        assert(pointerToResource && "Invalid image");
        HaxTexture result{};
        int width, height;
        auto image_data = stbi_load_from_memory((stbi_uc*)pointerToResource, sizeOfResource, &width, &height, NULL, 4);
        result.m_height = static_cast<float>(height);
        result.m_width = static_cast<float>(width);

        // Create texture
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Width = static_cast<UINT>(result.m_width);
        desc.Height = static_cast<UINT>(result.m_height);
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        ID3D11Texture2D* pTexture = NULL;
        D3D11_SUBRESOURCE_DATA subResource;
        subResource.pSysMem = image_data;
        subResource.SysMemPitch = desc.Width * 4;
        subResource.SysMemSlicePitch = 0;
        dx11::g_pDevice->CreateTexture2D(&desc, &subResource, &pTexture);

        // Create texture view
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
        dx11::g_pDevice->CreateShaderResourceView(pTexture, &srvDesc, (ID3D11ShaderResourceView**)&result.m_pTexture);
        pTexture->Release();
        stbi_image_free(image_data);

        return result;
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
                HAX_LOG_DEBUG("CreateCommandAllocator failed");
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

        HAX_LOG_DEBUG("[D3D12] oPresent address is {}", (void*)oPresent);
        HAX_LOG_DEBUG("[D3D12] oResizeBuffers is {}", (void*)oResizeBuffers);
        HAX_LOG_DEBUG("[D3D12] oExecuteCommandLists is {}", (void*)oExecuteCommandLists);

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
            params.graphicsApi = GraphicsApi_DirectX12;
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

            try
            {
                if (g_FnBackground)
                    g_FnBackground();

                if (g_MenuVisible && g_FnMenuRender)
                    g_FnMenuRender();
            }
            catch (System::Exception& ex)
            {
                System::String message = ex.GetMessage();
                HaxSdk::LogError(message ? message.UTF8() : "Exception without message");
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

    static HRESULT WINAPI HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, 
                                              DXGI_FORMAT newFormat, UINT swapChainFlags) {
        for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i) {
            if (g_mainRenderTargetResource[i]) {
                g_mainRenderTargetResource[i]->Release();
                g_mainRenderTargetResource[i] = NULL;
            }
        }
        return oResizeBuffers(pSwapChain, bufferCount, width, height, newFormat, swapChainFlags);
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
namespace vulkan
{
    static VkResult VKAPI_CALL HookedVkAcquireNextImageKHR(VkDevice device,
                                                    VkSwapchainKHR swapchain,
                                                    uint64_t timeout,
                                                    VkSemaphore semaphore,
                                                    VkFence fence,
                                                    uint32_t* pImageIndex) 
    {
        g_Device = device;

        return oVkAcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, pImageIndex);
    }

    static VkResult VKAPI_CALL HookedVkAcquireNextImage2KHR(VkDevice device,
        const VkAcquireNextImageInfoKHR* pAcquireInfo,
        uint32_t* pImageIndex) {
        g_Device = device;

        return oVkAcquireNextImage2KHR(device, pAcquireInfo, pImageIndex);
    }

    static VkResult VKAPI_CALL HookedVkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo) 
    {
        if (!ImGui::GetCurrentContext()) 
        {
            ImGuiContextParams params = { GraphicsApi_OpenGL, nullptr, nullptr, nullptr };
            InitImGuiContext(params);
            HAX_LOG("GAME USES VULKAN");
        }

        RenderImGui_Vulkan(queue, pPresentInfo);
        return oVkQueuePresentKHR(queue, pPresentInfo);
    }

    static VkResult VKAPI_CALL HookedVkCreateSwapchainKHR(VkDevice device,
                                                    const VkSwapchainCreateInfoKHR* pCreateInfo,
                                                    const VkAllocationCallbacks* pAllocator,
                                                    VkSwapchainKHR* pSwapchain) 
    {
        CleanupRenderTarget();
        g_ImageExtent = pCreateInfo->imageExtent;

        return oVkCreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);
    }

    static void Setup()
    {
        if (!CreateDevice()) 
        {
            HaxSdk::LogError("[!] CreateDeviceVK() failed.\n");
            return;
        }
            
        oVkAcquireNextImageKHR = (vkAcquireNextImageKHR_t)vkGetDeviceProcAddr(g_FakeDevice, "vkAcquireNextImageKHR");
        oVkAcquireNextImage2KHR = (vkAcquireNextImage2KHR_t)vkGetDeviceProcAddr(g_FakeDevice, "vkAcquireNextImage2KHR");
        oVkQueuePresentKHR = (vkQueuePresentKHR_t)vkGetDeviceProcAddr(g_FakeDevice, "vkQueuePresentKHR");
        oVkCreateSwapchainKHR = (vkCreateSwapchainKHR_t)vkGetDeviceProcAddr(g_FakeDevice, "vkCreateSwapchainKHR");

        if (g_FakeDevice) 
        {
            vkDestroyDevice(g_FakeDevice, g_Allocator);
            g_FakeDevice = NULL;
        }

        if (oVkAcquireNextImageKHR) {
            // Hook
            printf("[+] Vulkan: fnAcquireNextImageKHR: 0x%p\n", oVkAcquireNextImageKHR);
            printf("[+] Vulkan: fnAcquireNextImage2KHR: 0x%p\n", oVkAcquireNextImage2KHR);
            printf("[+] Vulkan: fnQueuePresentKHR: 0x%p\n", oVkQueuePresentKHR);
            printf("[+] Vulkan: fnCreateSwapchainKHR: 0x%p\n", oVkCreateSwapchainKHR);

            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourAttach(&oVkAcquireNextImageKHR, HookedVkAcquireNextImageKHR);
            DetourAttach(&oVkAcquireNextImage2KHR, HookedVkAcquireNextImage2KHR);
            DetourAttach(&oVkQueuePresentKHR, HookedVkQueuePresentKHR);
            DetourAttach(&oVkCreateSwapchainKHR, HookedVkCreateSwapchainKHR);
            DetourTransactionCommit();
        }
    }

    static void RenderImGui_Vulkan(VkQueue queue, const VkPresentInfoKHR* pPresentInfo) {
        if (!g_Device)
            return;

        VkQueue graphicQueue = VK_NULL_HANDLE;
        const bool queueSupportsGraphic = DoesQueueSupportGraphic(queue, &graphicQueue);


        for (uint32_t i = 0; i < pPresentInfo->swapchainCount; ++i) {
            VkSwapchainKHR swapchain = pPresentInfo->pSwapchains[i];
            if (g_Frames[0].Framebuffer == VK_NULL_HANDLE) {
                CreateRenderTarget(g_Device, swapchain);
            }

            ImGui_ImplVulkanH_Frame* fd = &g_Frames[pPresentInfo->pImageIndices[i]];
            ImGui_ImplVulkanH_FrameSemaphores* fsd = &g_FrameSemaphores[pPresentInfo->pImageIndices[i]];
            {
                vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE, ~0ull);
                vkResetFences(g_Device, 1, &fd->Fence);
            }
            {
                vkResetCommandBuffer(fd->CommandBuffer, 0);

                VkCommandBufferBeginInfo info = { };
                info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

                vkBeginCommandBuffer(fd->CommandBuffer, &info);
            }
            {
                VkRenderPassBeginInfo info = { };
                info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                info.renderPass = g_RenderPass;
                info.framebuffer = fd->Framebuffer;
                if (g_ImageExtent.width == 0 || g_ImageExtent.height == 0) {
                    // We don't know the window size the first time. So we just set it to 4K.
                    info.renderArea.extent.width = 3840;
                    info.renderArea.extent.height = 2160;
                }
                else {
                    info.renderArea.extent = g_ImageExtent;
                }

                vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
            }

            if (!ImGui::GetIO().BackendRendererUserData) {
                ImGui_ImplVulkan_InitInfo init_info = { };
                init_info.Instance = g_Instance;
                init_info.PhysicalDevice = g_PhysicalDevice;
                init_info.Device = g_Device;
                init_info.QueueFamily = g_QueueFamily;
                init_info.Queue = graphicQueue;
                init_info.PipelineCache = g_PipelineCache;
                init_info.DescriptorPool = g_DescriptorPool;
                init_info.Subpass = 0;
                init_info.MinImageCount = g_MinImageCount;
                init_info.ImageCount = g_MinImageCount;
                init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
                init_info.Allocator = g_Allocator;
                ImGui_ImplVulkan_Init(&init_info, g_RenderPass);

                ImGui_ImplVulkan_CreateFontsTexture(fd->CommandBuffer);
            }

            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            try
            {
                if (g_FnBackground)
                    g_FnBackground();

                if (g_MenuVisible && g_FnMenuRender)
                    g_FnMenuRender();
            }
            catch (System::Exception& ex)
            {
                System::String message = ex.GetMessage();
                HaxSdk::LogError(message ? message.UTF8() : "Exception without message");
            }

            ImGui::Render();

            // Record dear imgui primitives into command buffer
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), fd->CommandBuffer);

            // Submit command buffer
            vkCmdEndRenderPass(fd->CommandBuffer);
            vkEndCommandBuffer(fd->CommandBuffer);

            uint32_t waitSemaphoresCount = i == 0 ? pPresentInfo->waitSemaphoreCount : 0;
            if (waitSemaphoresCount == 0 && !queueSupportsGraphic) {
                constexpr VkPipelineStageFlags stages_wait = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
                {
                    VkSubmitInfo info = { };
                    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

                    info.pWaitDstStageMask = &stages_wait;

                    info.signalSemaphoreCount = 1;
                    info.pSignalSemaphores = &fsd->RenderCompleteSemaphore;

                    vkQueueSubmit(queue, 1, &info, VK_NULL_HANDLE);
                }
                {
                    VkSubmitInfo info = { };
                    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                    info.commandBufferCount = 1;
                    info.pCommandBuffers = &fd->CommandBuffer;

                    info.pWaitDstStageMask = &stages_wait;
                    info.waitSemaphoreCount = 1;
                    info.pWaitSemaphores = &fsd->RenderCompleteSemaphore;

                    info.signalSemaphoreCount = 1;
                    info.pSignalSemaphores = &fsd->ImageAcquiredSemaphore;

                    vkQueueSubmit(graphicQueue, 1, &info, fd->Fence);
                }
            }
            else {
                std::vector<VkPipelineStageFlags> stages_wait(waitSemaphoresCount, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

                VkSubmitInfo info = { };
                info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                info.commandBufferCount = 1;
                info.pCommandBuffers = &fd->CommandBuffer;

                info.pWaitDstStageMask = stages_wait.data();
                info.waitSemaphoreCount = waitSemaphoresCount;
                info.pWaitSemaphores = pPresentInfo->pWaitSemaphores;

                info.signalSemaphoreCount = 1;
                info.pSignalSemaphores = &fsd->ImageAcquiredSemaphore;

                vkQueueSubmit(graphicQueue, 1, &info, fd->Fence);
            }
        }
    }

    static bool CreateDevice() 
    {
        // Create Vulkan Instance
        {
            VkInstanceCreateInfo create_info = { };
            constexpr const char* instance_extension = "VK_KHR_surface";

            create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            create_info.enabledExtensionCount = 1;
            create_info.ppEnabledExtensionNames = &instance_extension;

            // Create Vulkan Instance without any debug feature
            vkCreateInstance(&create_info, g_Allocator, &g_Instance);
            HAX_LOG_DEBUG("[+] Vulkan: g_Instance: 0x {}\n", (void*)g_Instance);
        }

        // Select GPU
        {
            uint32_t gpu_count;
            vkEnumeratePhysicalDevices(g_Instance, &gpu_count, NULL);
            IM_ASSERT(gpu_count > 0);

            VkPhysicalDevice* gpus = new VkPhysicalDevice[sizeof(VkPhysicalDevice) * gpu_count];
            vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus);

            // If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
            // most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
            // dedicated GPUs) is out of scope of this sample.
            int use_gpu = 0;
            for (int i = 0; i < (int)gpu_count; ++i) {
                VkPhysicalDeviceProperties properties;
                vkGetPhysicalDeviceProperties(gpus[i], &properties);
                if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                    use_gpu = i;
                    break;
                }
            }

            g_PhysicalDevice = gpus[use_gpu];
            HAX_LOG_DEBUG("[+] Vulkan: g_PhysicalDevice: {}\n", (void*)g_PhysicalDevice);

            delete[] gpus;
        }

        // Select graphics queue family
        {
            uint32_t count;
            vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, NULL);
            g_QueueFamilies.resize(count);
            vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, g_QueueFamilies.data());
            for (uint32_t i = 0; i < count; ++i) {
                if (g_QueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    g_QueueFamily = i;
                    break;
                }
            }
            IM_ASSERT(g_QueueFamily != (uint32_t)-1);

            HAX_LOG_DEBUG("[+] Vulkan: g_QueueFamily: {}\n", g_QueueFamily);
        }

        // Create Logical Device (with 1 queue)
        {
            constexpr const char* device_extension = "VK_KHR_swapchain";
            constexpr const float queue_priority = 1.0f;

            VkDeviceQueueCreateInfo queue_info = { };
            queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_info.queueFamilyIndex = g_QueueFamily;
            queue_info.queueCount = 1;
            queue_info.pQueuePriorities = &queue_priority;

            VkDeviceCreateInfo create_info = { };
            create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            create_info.queueCreateInfoCount = 1;
            create_info.pQueueCreateInfos = &queue_info;
            create_info.enabledExtensionCount = 1;
            create_info.ppEnabledExtensionNames = &device_extension;

            vkCreateDevice(g_PhysicalDevice, &create_info, g_Allocator, &g_FakeDevice);

            HAX_LOG_DEBUG("[+] Vulkan: g_FakeDevice: {}\n", (void*)g_FakeDevice);
        }

        return true;
    }

    static bool DoesQueueSupportGraphic(VkQueue queue, VkQueue* pGraphicQueue) {
        for (uint32_t i = 0; i < g_QueueFamilies.size(); ++i) {
            const VkQueueFamilyProperties& family = g_QueueFamilies[i];
            for (uint32_t j = 0; j < family.queueCount; ++j) {
                VkQueue it = VK_NULL_HANDLE;
                vkGetDeviceQueue(g_Device, i, j, &it);

                if (pGraphicQueue && family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    if (*pGraphicQueue == VK_NULL_HANDLE) {
                        *pGraphicQueue = it;
                    }
                }

                if (queue == it && family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    return true;
                }
            }
        }

        return false;
    }

    static void CreateRenderTarget(VkDevice device, VkSwapchainKHR swapchain) {
        uint32_t uImageCount;
        vkGetSwapchainImagesKHR(device, swapchain, &uImageCount, NULL);

        VkImage backbuffers[8] = { };
        vkGetSwapchainImagesKHR(device, swapchain, &uImageCount, backbuffers);

        for (uint32_t i = 0; i < uImageCount; ++i) {
            g_Frames[i].Backbuffer = backbuffers[i];

            ImGui_ImplVulkanH_Frame* fd = &g_Frames[i];
            ImGui_ImplVulkanH_FrameSemaphores* fsd = &g_FrameSemaphores[i];
            {
                VkCommandPoolCreateInfo info = { };
                info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
                info.queueFamilyIndex = g_QueueFamily;

                vkCreateCommandPool(device, &info, g_Allocator, &fd->CommandPool);
            }
            {
                VkCommandBufferAllocateInfo info = { };
                info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                info.commandPool = fd->CommandPool;
                info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                info.commandBufferCount = 1;

                vkAllocateCommandBuffers(device, &info, &fd->CommandBuffer);
            }
            {
                VkFenceCreateInfo info = { };
                info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
                info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
                vkCreateFence(device, &info, g_Allocator, &fd->Fence);
            }
            {
                VkSemaphoreCreateInfo info = { };
                info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                vkCreateSemaphore(device, &info, g_Allocator, &fsd->ImageAcquiredSemaphore);
                vkCreateSemaphore(device, &info, g_Allocator, &fsd->RenderCompleteSemaphore);
            }
        }

        // Create the Render Pass
        {
            VkAttachmentDescription attachment = { };
            attachment.format = VK_FORMAT_B8G8R8A8_UNORM;
            attachment.samples = VK_SAMPLE_COUNT_1_BIT;
            attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference color_attachment = { };
            color_attachment.attachment = 0;
            color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass = { };
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &color_attachment;

            VkRenderPassCreateInfo info = { };
            info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            info.attachmentCount = 1;
            info.pAttachments = &attachment;
            info.subpassCount = 1;
            info.pSubpasses = &subpass;

            vkCreateRenderPass(device, &info, g_Allocator, &g_RenderPass);
        }

        // Create The Image Views
        {
            VkImageViewCreateInfo info = { };
            info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            info.format = VK_FORMAT_B8G8R8A8_UNORM;

            info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            info.subresourceRange.baseMipLevel = 0;
            info.subresourceRange.levelCount = 1;
            info.subresourceRange.baseArrayLayer = 0;
            info.subresourceRange.layerCount = 1;

            for (uint32_t i = 0; i < uImageCount; ++i) {
                ImGui_ImplVulkanH_Frame* fd = &g_Frames[i];
                info.image = fd->Backbuffer;

                vkCreateImageView(device, &info, g_Allocator, &fd->BackbufferView);
            }
        }

        // Create Framebuffer
        {
            VkImageView attachment[1];
            VkFramebufferCreateInfo info = { };
            info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            info.renderPass = g_RenderPass;
            info.attachmentCount = 1;
            info.pAttachments = attachment;
            info.layers = 1;

            for (uint32_t i = 0; i < uImageCount; ++i) {
                ImGui_ImplVulkanH_Frame* fd = &g_Frames[i];
                attachment[0] = fd->BackbufferView;

                vkCreateFramebuffer(device, &info, g_Allocator, &fd->Framebuffer);
            }
        }

        if (!g_DescriptorPool) // Create Descriptor Pool.
        {
            constexpr VkDescriptorPoolSize pool_sizes[] =
            {
                {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000} };

            VkDescriptorPoolCreateInfo pool_info = { };
            pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
            pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
            pool_info.pPoolSizes = pool_sizes;

            vkCreateDescriptorPool(device, &pool_info, g_Allocator, &g_DescriptorPool);
        }
    }

    static void CleanupRenderTarget() {
        for (uint32_t i = 0; i < RTL_NUMBER_OF(g_Frames); ++i) {
            if (g_Frames[i].Fence) {
                vkDestroyFence(g_Device, g_Frames[i].Fence, g_Allocator);
                g_Frames[i].Fence = VK_NULL_HANDLE;
            }
            if (g_Frames[i].CommandBuffer) {
                vkFreeCommandBuffers(g_Device, g_Frames[i].CommandPool, 1, &g_Frames[i].CommandBuffer);
                g_Frames[i].CommandBuffer = VK_NULL_HANDLE;
            }
            if (g_Frames[i].CommandPool) {
                vkDestroyCommandPool(g_Device, g_Frames[i].CommandPool, g_Allocator);
                g_Frames[i].CommandPool = VK_NULL_HANDLE;
            }
            if (g_Frames[i].BackbufferView) {
                vkDestroyImageView(g_Device, g_Frames[i].BackbufferView, g_Allocator);
                g_Frames[i].BackbufferView = VK_NULL_HANDLE;
            }
            if (g_Frames[i].Framebuffer) {
                vkDestroyFramebuffer(g_Device, g_Frames[i].Framebuffer, g_Allocator);
                g_Frames[i].Framebuffer = VK_NULL_HANDLE;
            }
        }

        for (uint32_t i = 0; i < RTL_NUMBER_OF(g_FrameSemaphores); ++i) {
            if (g_FrameSemaphores[i].ImageAcquiredSemaphore) {
                vkDestroySemaphore(g_Device, g_FrameSemaphores[i].ImageAcquiredSemaphore, g_Allocator);
                g_FrameSemaphores[i].ImageAcquiredSemaphore = VK_NULL_HANDLE;
            }
            if (g_FrameSemaphores[i].RenderCompleteSemaphore) {
                vkDestroySemaphore(g_Device, g_FrameSemaphores[i].RenderCompleteSemaphore, g_Allocator);
                g_FrameSemaphores[i].RenderCompleteSemaphore = VK_NULL_HANDLE;
            }
        }
    }
}