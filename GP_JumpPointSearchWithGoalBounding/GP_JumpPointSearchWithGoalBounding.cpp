// GP_JumpPointSearchWithGoalBounding.cpp : Defines the entry point for the application.
//

#include "GP_JumpPointSearchWithGoalBounding.h"
#include "GridGraph.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

Engine* g_MyEnginePtr{};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

    g_MyEnginePtr = new Engine(hInstance, nCmdShow);
    g_MyEnginePtr->m_Width = 760;
    g_MyEnginePtr->m_Height = 760;
    return g_MyEnginePtr->Run(new GridGraph(10,10,g_MyEnginePtr->m_Width/12, g_MyEnginePtr));
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return g_MyEnginePtr->HandleEvent(hWnd, message, wParam, lParam);
}

Engine::Engine(HINSTANCE hInstance, int nCmdShow) : m_Instance(hInstance), m_Show(nCmdShow)
{
    m_Width = 760;
    m_Height = 760;

    // (2) Register the window class (= choose the type of window you want)
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_Instance;
    wcex.hIcon = LoadIcon(m_Instance, MAKEINTRESOURCE(IDC_GPJUMPPOINTSEARCHWITHGOALBOUNDING));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"JPS+";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GPJUMPPOINTSEARCHWITHGOALBOUNDING));

    RegisterClassExW(&wcex);
}

Engine::~Engine()
{
    delete g_MyEnginePtr;
    g_MyEnginePtr = nullptr;

    delete m_pGridGraph;
    m_pGridGraph = nullptr;
}

int Engine::Run(GridGraph* gridGraph)
{
    m_pGridGraph = gridGraph;
    // (3) Create the window, store the handle, check if all went well
    HWND hWnd = CreateWindowW(L"JPS+",
        L"JumpPointSearchWithGoalBounding",        // window title
        WS_OVERLAPPEDWINDOW,    // window style
        CW_USEDEFAULT,          // default x
        CW_USEDEFAULT,          // default y
        m_Width,          // default width
        m_Height,          // default height
        nullptr,
        nullptr,
        m_Instance,
        nullptr);

    if (!hWnd)
    {
        MessageBox(NULL, L"Failure to create program window", L"Error", MB_ICONERROR);

        // this would be a nice place for a throw
    }

    // (4) Set the window to show
    ShowWindow(hWnd, m_Show);	    // set the window to show mode
    UpdateWindow(hWnd);	            // update

    // (5) load keyboard shortcuts, start the Windows message loop
    HACCEL hAccelTable = LoadAccelerators(m_Instance, MAKEINTRESOURCE(IDC_GPJUMPPOINTSEARCHWITHGOALBOUNDING));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // (6) Close with received Quit message information
    return (int)msg.wParam;
}

LRESULT Engine::HandleEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    m_hWnd = hWnd;

    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        m_hDC = BeginPaint(hWnd, &ps);
        m_pGridGraph->DrawGrid();
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

void Engine::SetColor(int r, int g, int b)
{
    m_Color = COLORREF{ RGB(r, g, b) };
}


void Engine::DrawLine(int x1, int y1, int x2, int y2)
{
    HPEN pen = CreatePen(PS_SOLID, 1, m_Color);
    HPEN oldPen = (HPEN)SelectObject(m_hDC, pen);

    MoveToEx(m_hDC, x1, y1, 0);
    LineTo(m_hDC, x2, y2);

    SelectObject(m_hDC, oldPen);
    DeleteObject(pen);
}

void Engine::DrawRect(int x1, int y1, int width, int height)
{

    HPEN pen = CreatePen(PS_SOLID, 1, m_Color);
    HPEN oldPen = (HPEN)SelectObject(m_hDC, pen);

    MoveToEx(m_hDC, x1, y1, 0);
    LineTo(m_hDC, x1 + width, y1);
    LineTo(m_hDC, x1 + width, y1 + height);
    LineTo(m_hDC, x1, y1 + height);
    LineTo(m_hDC, x1, y1);
    /*MoveToEx(m_hDC, x1, y1, 0);
    LineTo(m_hDC, x1, y1 + height);
    MoveToEx(m_hDC, x1 + width, y1 + height, 0);
    LineTo(m_hDC, x1 + width, y1);
    MoveToEx(m_hDC, x1 + width, y1 + height, 0);
    LineTo(m_hDC, x1, y1 + height);*/

    SelectObject(m_hDC, oldPen);
    DeleteObject(pen);
}

void Engine::FillRect(int x1, int y1, int width, int height)
{
    HBRUSH brush = CreateSolidBrush(m_Color);
    HBRUSH oldBrush = (HBRUSH)SelectObject(m_hDC, brush);

    Rectangle(m_hDC, x1, y1 + height, x1 + width, y1);

    SelectObject(m_hDC, oldBrush);
    DeleteObject(brush);
}

void Engine::FillEllipse(int x1, int y1, int radiusX, int radiusY)
{
    HBRUSH brush = CreateSolidBrush(m_Color);
    HBRUSH oldBrush = (HBRUSH)SelectObject(m_hDC, brush);

    Ellipse(m_hDC, x1 - radiusX / 2, y1 - radiusY / 2, x1 + radiusX / 2, y1 + radiusY / 2);

    SelectObject(m_hDC, oldBrush);
    DeleteObject(brush);
}

