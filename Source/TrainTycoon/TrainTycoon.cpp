// TrainTycoon.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "TrainTycoon.h"
#include "ctx_logger.h"

#include <assert.h>
#include <wrl.h>

#include <GameInput.h>

#define MAX_LOADSTRING 128

extern "C"
{
    extern const uint32_t D3D12SDKVersion = 614;

    extern LPCSTR D3D12SDKPath = ".\\D3D12\\";
}

// Global Variables:
static _TCHAR 
    g_zTitle[MAX_LOADSTRING],                  // The title bar text
    g_zWindowClass[MAX_LOADSTRING];            // the main window class name
static LONGLONG
    g_qpcFrequency,
    g_qpcStartCounter,
    g_qpcCurrentCounter;

// Forward declarations of functions included in this code module:
static ATOM             RegisterMainClass(_In_ HINSTANCE hInstance);
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

extern void ExitGame(int code) {
    PostQuitMessage(code);
}

HRESULT RenderSetup();

HRESULT RenderSurfaceLoad(_In_ HWND);

HRESULT RenderResize(UINT width, UINT height);

void RenderDraw();

HRESULT RenderUpdate();

void RenderTeardown();

_Use_decl_annotations_
extern int APIENTRY _tWinMain(HINSTANCE hInstance,
                              HINSTANCE hPrevInstance,
                              LPTSTR    lpCmdLine,
                              INT       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    HRESULT hr;

    RenderSetup();

    // Initialized counter
    QueryPerformanceFrequency((PLARGE_INTEGER)&g_qpcFrequency);
    if (!g_qpcFrequency)
        return -1;
    QueryPerformanceCounter((PLARGE_INTEGER)&g_qpcStartCounter);
    if (!g_qpcStartCounter)
        return -1;
    else g_qpcCurrentCounter = g_qpcStartCounter;

    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, g_zTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_TRAINTYCOON, g_zWindowClass, MAX_LOADSTRING);
    const ATOM aCls = RegisterMainClass(hInstance);
    if (!aCls)
        return -1;

    const HWND hWnd = CreateWindowEx(0L, (LPCTSTR)aCls, g_zTitle, WS_OVERLAPPEDWINDOW,
        // X, Y
        CW_USEDEFAULT, 0, 
        // Width * Height
        CW_USEDEFAULT, 0, 
        NULL, NULL, hInstance, NULL);
    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);


    // Perform application initialization:
   
    Microsoft::WRL::ComPtr<IGameInput> pGameInput;
    if (FAILED(hr = GameInputCreate(pGameInput.ReleaseAndGetAddressOf())))
    {
        __debugbreak();
        return FALSE;
    }

    const HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TRAINTYCOON));

    // Main message loop:
    MSG msg{};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            continue; // Continue until message queue is empty.
        }

        /*
         * Loop Code
         */


        RenderDraw();
    }

    // Epilogue
    RenderTeardown();


    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
_Use_decl_annotations_
ATOM RegisterMainClass(HINSTANCE hInstance)
{
    const WNDCLASSEX wcex = {
        .cbSize = sizeof(WNDCLASSEX),

        .style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
        .lpfnWndProc    = WndProc,
        .cbClsExtra     = 0,
        .cbWndExtra     = 0,
        .hInstance      = hInstance,
        .hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAINTYCOON)),
        .hCursor        = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground  = (HBRUSH)COLOR_WINDOWFRAME,
        .lpszMenuName   = MAKEINTRESOURCEW(IDC_TRAINTYCOON),
        .lpszClassName  = g_zWindowClass,
        .hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL)),
    };
    return RegisterClassEx(&wcex);
}



//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    static bool s_bMaximized = false;
    static bool s_bMinimized = false;
    static bool s_bSuspended = false;

    switch (message)
    {
    case WM_COMMAND:
    {
        const int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }

    // case WM_PAINT:
    // {
    //     PAINTSTRUCT ps;
    //     HDC hdc = BeginPaint(hWnd, &ps);
    //     // TODO: Add any drawing code that uses hdc here...
    //     EndPaint(hWnd, &ps);
    // }
    // break;

    case WM_SIZE:
    {
        const WORD 
            width = LOWORD(lParam),
            height = HIWORD(lParam),
            eSize = (WORD)wParam;
        switch (eSize)
        {
        case SIZE_MAXIMIZED:
        {
            s_bMaximized = true;
            s_bMinimized = false;
            // Remove taskbar
            
            break;
        }
        case SIZE_RESTORED:
        {
            
            break;
        }
        case SIZE_MINIMIZED:
        {
            
            s_bSuspended = true;
            s_bMinimized = true;
            s_bMaximized = false;
            break;
        }
        default:
            assert(eSize == SIZE_MAXSHOW || eSize == SIZE_MAXHIDE);
            break;
        }

        RenderResize(width, height);

        break;
    }
    case WM_POWERBROADCAST:
    {
        const WORD eEvent = (WORD)wParam;
        switch (eEvent)
        {
        case PBT_APMPOWERSTATUSCHANGE:
        {
            SYSTEM_POWER_STATUS powerStatus;
            GetSystemPowerStatus(&powerStatus);
            // TODO
            break;
        }
        case PBT_APMRESUMEAUTOMATIC:
        {
            // System is resuming from sleep or hibernation. 
            // This event is delivered every time the system resumes and does not indicate whether a user is present.
            break;
        }
        case PBT_APMRESUMESUSPEND:
        {
            // System has resumed operation after being suspended.

            s_bSuspended = false;
            break;
        }
        case PBT_APMSUSPEND:
        {
            // Computer is about to enter a suspended state

            s_bSuspended = true;
            break;
        }
        case PBT_POWERSETTINGCHANGE:
        {
            const PPOWERBROADCAST_SETTING pSettings = (PPOWERBROADCAST_SETTING)lParam;

            break;
        }

        default:
            break;
        }

        break;
    }
    case WM_DESTROY:
        ExitGame(0);
        break;
    case WM_CREATE:
    {
        LPCREATESTRUCT pCS = (LPCREATESTRUCT)lParam;

        RenderSurfaceLoad(hWnd);
        break;
    }
    default:
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void ctx_logger_output(enum ctx_logger_severity, const wchar_t* zMsg, size_t msgSize)
{
    OutputDebugStringW(zMsg);
}


/*
* 
* 
* 
*/
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;
constexpr UINT c_FrameCount = 3;
constexpr DXGI_FORMAT c_RenderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
constexpr DXGI_FORMAT c_DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;
constexpr float c_ClearColor[4] = { .2f, .2f, .8f, 1.f };

ComPtr<ID3D12Device10>      device;
ComPtr<ID3D12CommandQueue>  queue;
ComPtr<IDXGISwapChain4>     swapChain;

ComPtr<ID3D12CommandAllocator>      allocators[c_FrameCount];
ComPtr<ID3D12GraphicsCommandList>   cmdLists[c_FrameCount];

ComPtr<ID3D12DescriptorHeap> rtvHeap;
ComPtr<ID3D12DescriptorHeap> dsvHeap;

ComPtr<ID3D12Resource> renderTargetBuffers[c_FrameCount];
ComPtr<ID3D12Resource> depthStencilBuffer;

ComPtr<ID3D12Fence> fence;
Microsoft::WRL::Wrappers::Event hFenceEvent;
UINT64 fenceValues[c_FrameCount] = { 0,0,0 };

UINT ixCurrentFrame;
BOOL bTearing = false;
UINT rtvHeapIncSize;
UINT dsvHeapIncSize;
RECT scissor;
D3D12_VIEWPORT viewport;



void recordCmdList(ID3D12GraphicsCommandList* cmdlist, UINT ixFrame)
{

    const auto pre_transition = CD3DX12_RESOURCE_BARRIER::Transition(renderTargetBuffers[ixFrame].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);   
    cmdlist->ResourceBarrier(1, &pre_transition);

    const auto handleRtv = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeap->GetCPUDescriptorHandleForHeapStart(), ixFrame, rtvHeapIncSize);
    const auto handleDsv = CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvHeap->GetCPUDescriptorHandleForHeapStart());
    
    cmdlist->RSSetViewports(1, &viewport);
    cmdlist->RSSetScissorRects(1, &scissor);
    cmdlist->OMSetRenderTargets(1, &handleRtv, false, &handleDsv);

    cmdlist->ClearRenderTargetView(handleRtv, c_ClearColor, 0, nullptr);
    cmdlist->ClearDepthStencilView(handleDsv, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
        
    const auto post_transition = CD3DX12_RESOURCE_BARRIER::Transition(renderTargetBuffers[ixFrame].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    cmdlist->ResourceBarrier(1, &post_transition);

}

void moveToNextFrame(ID3D12Fence* fence, UINT ixFrame)
{
    UINT64 fenceValue = fenceValues[ixFrame];


    fenceValues[ixFrame] = fenceValue +1 ;

    if (fence->GetCompletedValue() < fenceValue)
    {
        WaitForSingleObject(hFenceEvent.Get(), INFINITE);
        ResetEvent(hFenceEvent.Get());
    }
    
    fence->SetEventOnCompletion(fenceValues[ixFrame], hFenceEvent.Get());

}


HRESULT RenderSetup() 
{
    HRESULT hr = S_OK;

#ifdef _DEBUG
    ComPtr<ID3D12Debug6> pDebug;
    if (FAILED(hr = D3D12GetDebugInterface(IID_PPV_ARGS(pDebug.ReleaseAndGetAddressOf())))) {
        __debugbreak();
        return hr;
    }
#endif // !_DEBUG

    //
    // TODO: Select Device
    //
    if (FAILED(hr = D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(device.ReleaseAndGetAddressOf())))) {
        __debugbreak();
        return hr;
    }

    const D3D12_COMMAND_QUEUE_DESC queueDesc = {
        .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH,
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE
    };
    device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(queue.ReleaseAndGetAddressOf()));

    /*
     * Descriptor Heaps
     */
    
    const D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        .NumDescriptors = c_FrameCount,
    };
    device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(rtvHeap.ReleaseAndGetAddressOf()));
    rtvHeapIncSize = device->GetDescriptorHandleIncrementSize(rtvDesc.Type);

    const D3D12_DESCRIPTOR_HEAP_DESC dsvDesc = {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
        .NumDescriptors = 1,
    };
    device->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(dsvHeap.ReleaseAndGetAddressOf()));
    dsvHeapIncSize = device->GetDescriptorHandleIncrementSize(dsvDesc.Type);

    /*
    * Synchronization objects
    */

    hFenceEvent.Attach(CreateEvent(nullptr, FALSE, FALSE, nullptr));

    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.ReleaseAndGetAddressOf()));


   /*
   * Command Allocator and Lists
   */
    for (UINT ix = 0; ix < c_FrameCount; ix++) {
        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(allocators[ix].ReleaseAndGetAddressOf()));
        device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(cmdLists[ix].ReleaseAndGetAddressOf()));
    }

    return hr;
}

_Use_decl_annotations_
HRESULT RenderSurfaceLoad(HWND hWnd) 
{

    assert(queue.Get() != nullptr);


    UINT factoryFlags = 0;
#ifdef _DEBUG
    factoryFlags += DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG

    ComPtr<IDXGIFactory7> pFactory;
    CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(pFactory.GetAddressOf()));


    pFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &bTearing, sizeof bTearing); 

    const DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
        .Width = 256,
        .Height = 256,
        .Format = c_RenderTargetFormat,
        .SampleDesc = {.Count = 1},
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = c_FrameCount,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
        .Flags = (bTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u)
    };
    ComPtr<IDXGISwapChain1> pDummy;
    pFactory->CreateSwapChainForHwnd(queue.Get(), hWnd, &swapChainDesc, nullptr, nullptr, pDummy.GetAddressOf());

    pDummy.As(&swapChain);

    return E_NOTIMPL;
}

HRESULT RenderResize(UINT width, UINT height) {
    for (auto& rt : renderTargetBuffers) 
        rt.Reset();

    swapChain->ResizeBuffers(c_FrameCount, width, height, c_RenderTargetFormat, bTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);

    device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(c_DepthStencilFormat, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &CD3DX12_CLEAR_VALUE(c_DepthStencilFormat, 1.f, 0),
        IID_PPV_ARGS(depthStencilBuffer.ReleaseAndGetAddressOf())
    );

    auto dsvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvHeap->GetCPUDescriptorHandleForHeapStart());
    device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, dsvHandle);

    auto rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT ix = 0; ix < c_FrameCount; ix++) {

        swapChain->GetBuffer(ix, IID_PPV_ARGS(renderTargetBuffers[ix].GetAddressOf()));
        device->CreateRenderTargetView(renderTargetBuffers[ix].Get(), nullptr, rtvHandle);

        rtvHandle.Offset(1, rtvHeapIncSize);
    }

    ixCurrentFrame = swapChain->GetCurrentBackBufferIndex();

    scissor = CD3DX12_RECT(0, 0, LONG(width), LONG(height));

    viewport = CD3DX12_VIEWPORT(0.f, 0.f, FLOAT(width), FLOAT(height));

    return S_OK;
}

void RenderDraw() {

    /*
    * 
    */

    allocators[ixCurrentFrame]->Reset();
    // todo: change initial pipeline.
    cmdLists[ixCurrentFrame]->Reset(allocators[ixCurrentFrame].Get(), nullptr);

    recordCmdList(cmdLists[ixCurrentFrame].Get(), ixCurrentFrame);

    cmdLists[ixCurrentFrame]->Close();

    ID3D12CommandList* const executables[] = { cmdLists[ixCurrentFrame].Get() };
    
    queue->ExecuteCommandLists(_countof(executables), executables);

    swapChain->Present(1, 0);


    /*
    * Move to next frame
    */

    // Schedule a Signal command in the queue.
    const UINT64 currentFenceValue = fenceValues[ixCurrentFrame];
    queue->Signal(fence.Get(), currentFenceValue);

    // Update the frame index.
    ixCurrentFrame = swapChain->GetCurrentBackBufferIndex();

    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (fence->GetCompletedValue() < fenceValues[ixCurrentFrame])
    {
        fence->SetEventOnCompletion(fenceValues[ixCurrentFrame], hFenceEvent.Get());
        WaitForSingleObjectEx(hFenceEvent.Get(), INFINITE, FALSE);
    }

    // Set the fence value for the next frame.
    fenceValues[ixCurrentFrame] = currentFenceValue + 1;
}

HRESULT RenderUpdate() {
    return E_NOTIMPL;
}

void RenderTeardown() {

}