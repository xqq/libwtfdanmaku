#include "SampleWindow.hpp"
#include <CommCtrl.h>

#pragma comment (lib, "Comctl32.lib")

namespace WTFDanmaku {

    SampleWindow::SampleWindow(HINSTANCE hInst, int nCmdShow) : m_hInstance(hInst), m_nCmdShow(nCmdShow) { }

    SampleWindow::~SampleWindow() {

    }

    int SampleWindow::Initialize(DWORD dwExStyle) {
        INITCOMMONCONTROLSEX iccex = { 0 };
        iccex.dwSize = sizeof(iccex);
        iccex.dwICC = ICC_WIN95_CLASSES;
        InitCommonControlsEx(&iccex);

        RegisterWindowClass();

        m_hWindow = CreateWindowEx(dwExStyle, m_WindowClassName, L"WTFDanmaku Test", WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, m_hInstance, this);

        if (!m_hWindow) {
            return FALSE;
        }

        ShowWindow(m_hWindow, m_nCmdShow);
        UpdateWindow(m_hWindow);

        return TRUE;
    }

    ATOM SampleWindow::RegisterWindowClass() {
        WNDCLASSEX wcex;
        memset(&wcex, 0, sizeof(wcex));

        wcex.cbSize = sizeof(wcex);

        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = &SampleWindow::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = m_hInstance;
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = m_WindowClassName;

        return RegisterClassEx(&wcex);
    }

    void SampleWindow::SetHitTestOverEnabled(bool enabled) {
        if (m_hWindow) {
            long currentLong = GetWindowLong(m_hWindow, GWL_EXSTYLE);
            if (enabled) {
                SetWindowLong(m_hWindow, GWL_EXSTYLE, currentLong | WS_EX_LAYERED | WS_EX_TRANSPARENT);
            }
            else {
                SetWindowLong(m_hWindow, GWL_EXSTYLE, currentLong & ~(WS_EX_LAYERED | WS_EX_TRANSPARENT));
            }
        }
    }

    int SampleWindow::Run() {
        MSG msg = { 0 };

        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return static_cast<int>(msg.wParam);
    }

    LRESULT CALLBACK SampleWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        SampleWindow* window = nullptr;

        if (WM_NCCREATE == message) {
            LPCREATESTRUCT cs = reinterpret_cast<LPCREATESTRUCT>(lParam);
            window = static_cast<SampleWindow*>(cs->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        } else {
            window = reinterpret_cast<SampleWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }

        if (window == nullptr)
            return DefWindowProc(hWnd, message, wParam, lParam);

        switch (message) {
            case WM_CREATE: {
                window->m_hWindow = hWnd;
                break;
            }
            case WM_COMMAND:
                break;
            case WM_PAINT:
                break;
            case WM_LBUTTONDOWN:
                break;
            case WM_LBUTTONUP:
                break;
            case WM_DESTROY:
                PostQuitMessage(0);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }

        return 0;
    }

}