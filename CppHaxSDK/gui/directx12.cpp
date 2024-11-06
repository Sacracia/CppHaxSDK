#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_4.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "third_party/detours/x64/detours.lib")

#include <memory>

#include "directx12.h"
#include "../logger/logger.h"

#include "../third_party/imgui/backend/imgui_impl_dx12.h"
#include "../third_party/imgui/backend/imgui_impl_win32.h"
#include "../third_party/detours/x64/detours.h"

using present_t = HRESULT(WINAPI*)(IDXGISwapChain3*, UINT, UINT);
using resizeBuffers_t = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
using executeCommandLists_t = void(WINAPI*)(ID3D12CommandQueue*, UINT, ID3D12CommandList*);

// Data
static int const NUM_BACK_BUFFERS = 3;
static IDXGIFactory4* g_dxgiFactory = NULL;
static ID3D12Device* g_pd3dDevice = NULL;
static ID3D12DescriptorHeap* g_pd3dRtvDescHeap = NULL;
static ID3D12DescriptorHeap* g_pd3dSrvDescHeap = NULL;
static ID3D12CommandQueue* g_pd3dCommandQueue = NULL;
static ID3D12GraphicsCommandList* g_pd3dCommandList = NULL;
static IDXGISwapChain3* g_pSwapChain = NULL;
static ID3D12CommandAllocator* g_commandAllocators[NUM_BACK_BUFFERS] = { };
static ID3D12Resource* g_mainRenderTargetResource[NUM_BACK_BUFFERS] = { };
static D3D12_CPU_DESCRIPTOR_HANDLE g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = { };
static present_t oPresent;
static resizeBuffers_t oResizeBuffers;
static executeCommandLists_t oExecuteCommandLists;

static HRESULT WINAPI hkPresent(IDXGISwapChain3* pSwapChain,
    UINT SyncInterval,
    UINT Flags);
static HRESULT WINAPI hkResizeBuffers(IDXGISwapChain* pSwapChain,
    UINT BufferCount,
    UINT Width,
    UINT Height,
    DXGI_FORMAT NewFormat,
    UINT SwapChainFlags);
static void WINAPI hkExecuteCommandLists(ID3D12CommandQueue* pCommandQueue,
    UINT NumCommandLists,
    ID3D12CommandList* ppCommandLists);
static void CreateRenderTarget(IDXGISwapChain* pSwapChain);

void Hook() {
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
    if (g_dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, GetConsoleWindow(), &sd, NULL, NULL, &swapChain1) != S_OK)
        return;
    if (swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
        return;
    swapChain1->Release();

    LOG_INFO << "[+] DirectX12: g_pd3dDevice: " << g_pd3dDevice << LOG_FLUSH;
    LOG_INFO << "[+] DirectX12: g_dxgiFactory: " << g_dxgiFactory << LOG_FLUSH;
    LOG_INFO << "[+] DirectX12: g_pd3dCommandQueue: " << g_pd3dCommandQueue << LOG_FLUSH;
    LOG_INFO << "[+] DirectX12: g_pSwapChain: " << g_pSwapChain << LOG_FLUSH;

    // Hook
    void** pVTable = *reinterpret_cast<void***>(g_pSwapChain);
    void** pCommandQueueVTable = *reinterpret_cast<void***>(g_pd3dCommandQueue);

    oPresent = (present_t)pVTable[8];
    oResizeBuffers = (resizeBuffers_t)pVTable[13];
    oExecuteCommandLists = (executeCommandLists_t)pCommandQueueVTable[10];

    LOG_INFO << "[+] DirectX12: fnPresent: " << oPresent << LOG_FLUSH;
    LOG_INFO << "[+] DirectX12: fnResizeBuffers: " << oResizeBuffers << LOG_FLUSH;
    LOG_INFO << "[+] DirectX12: fnExecuteCommandLists: " << oExecuteCommandLists << LOG_FLUSH;

    if (g_pd3dCommandQueue) {
        g_pd3dCommandQueue->Release();
        g_pd3dCommandQueue = NULL;
    }
    for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i)
        if (g_mainRenderTargetResource[i]) {
            g_mainRenderTargetResource[i]->Release();
            g_mainRenderTargetResource[i] = NULL;
        }

    if (g_pSwapChain) {
        g_pSwapChain->Release();
        g_pSwapChain = NULL;
    }
    for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i)
        if (g_commandAllocators[i]) {
            g_commandAllocators[i]->Release();
            g_commandAllocators[i] = NULL;
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

    DetourTransactionBegin();
    DetourAttach(&(PVOID&)oPresent, hkPresent);
    DetourAttach(&(PVOID&)oResizeBuffers, hkResizeBuffers);
    DetourAttach(&(PVOID&)oExecuteCommandLists, hkExecuteCommandLists);
    DetourTransactionCommit();
    LOG_INFO << "HOOKED" << LOG_FLUSH;
}

static HRESULT WINAPI hkPresent(IDXGISwapChain3* pSwapChain,
    UINT SyncInterval,
    UINT Flags) {
    static bool inited = false;
    if (!inited) {
        DXGI_SWAP_CHAIN_DESC desc;
        pSwapChain->GetDesc(&desc);
        HWND hwnd = desc.OutputWindow;
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);
        inited = true;
    }

    if (!ImGui::GetIO().BackendRendererUserData) {
        if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&g_pd3dDevice)))) {
            {
                D3D12_DESCRIPTOR_HEAP_DESC desc = { };
                desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                desc.NumDescriptors = NUM_BACK_BUFFERS;
                desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                desc.NodeMask = 1;
                if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
                    return oPresent(pSwapChain, SyncInterval, Flags);

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
                    return oPresent(pSwapChain, SyncInterval, Flags);
            }

            for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i)
                if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_commandAllocators[i])) != S_OK)
                    return oPresent(pSwapChain, SyncInterval, Flags);

            if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_commandAllocators[0], NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
                g_pd3dCommandList->Close() != S_OK)
                return oPresent(pSwapChain, SyncInterval, Flags);

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

        ImGui::ShowDemoWindow();

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

    return oPresent(pSwapChain, SyncInterval, Flags);
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

static HRESULT WINAPI hkResizeBuffers(IDXGISwapChain* pSwapChain,
    UINT BufferCount,
    UINT Width,
    UINT Height,
    DXGI_FORMAT NewFormat,
    UINT SwapChainFlags) {
    for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i)
        if (g_mainRenderTargetResource[i]) {
            g_mainRenderTargetResource[i]->Release();
            g_mainRenderTargetResource[i] = NULL;
        }

    return oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

static void WINAPI hkExecuteCommandLists(ID3D12CommandQueue* pCommandQueue,
    UINT NumCommandLists,
    ID3D12CommandList* ppCommandLists) {
    if (!g_pd3dCommandQueue) {
        g_pd3dCommandQueue = pCommandQueue;
    }

    return oExecuteCommandLists(pCommandQueue, NumCommandLists, ppCommandLists);
}
