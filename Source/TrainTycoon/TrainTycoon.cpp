// TrainTycoon.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "TrainTycoon.h"

#include <assert.h>
#include <wrl.h>

#include <GameInput.h>

#define MAX_LOADSTRING 128

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


    }

    // Epilogue



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
            eSize = wParam;
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
