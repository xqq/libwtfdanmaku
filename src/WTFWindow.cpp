#include <Windows.h>
#ifndef _WTF_BUILD_UWP
#include <CommCtrl.h>
#endif
#include "../include/WTFWindow.hpp"

#ifndef _WTF_BUILD_UWP

#pragma comment (lib, "Comctl32.lib")

namespace WTFDanmaku {

    WTFWindow::WTFWindow(HINSTANCE hInst, int nCmdShow) : m_hInstance(hInst), m_nCmdShow(nCmdShow) { }

    WTFWindow::~WTFWindow() {

    }

    int WTFWindow::Initialize(DWORD dwExStyle, int width, int height, const wchar_t* title) {
        INITCOMMONCONTROLSEX iccex = { 0 };
        iccex.dwSize = sizeof(iccex);
        iccex.dwICC = ICC_WIN95_CLASSES;
        InitCommonControlsEx(&iccex);

        RegisterWindowClass();

        if (title == nullptr) {
            title = L"WTFDanmaku SampleWindow";
        }
        m_hWindow = CreateWindowEx(dwExStyle, m_WindowClassName, title, WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, m_hInstance, this);

        if (!m_hWindow) {
            return FALSE;
        }

        ShowWindow(m_hWindow, m_nCmdShow);
        UpdateWindow(m_hWindow);

        return TRUE;
    }

    void WTFWindow::SetCustomWindowProc(WNDPROC proc) {
        m_CustomWndProc = proc;
    }

    ATOM WTFWindow::RegisterWindowClass() {
        WNDCLASSEX wcex;
        memset(&wcex, 0, sizeof(wcex));

        wcex.cbSize = sizeof(wcex);

        wcex.style = CS_HREDRAW | CS_VREDRAW;
        if (m_CustomWndProc) {
            wcex.lpfnWndProc = m_CustomWndProc;
        } else {
            wcex.lpfnWndProc = &WTFWindow::WndProc;
        }
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = m_hInstance;
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hIcon = LoadIcon(m_hInstance, IDI_APPLICATION);
        wcex.hIconSm = LoadIcon(m_hInstance, IDI_APPLICATION);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = m_WindowClassName;

        return RegisterClassEx(&wcex);
    }

    void WTFWindow::SetHitTestOverEnabled(bool enabled) {
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

    int WTFWindow::Run() {
        MSG msg = { 0 };

        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return static_cast<int>(msg.wParam);
    }

    HWND WTFWindow::GetHwnd() {
        return m_hWindow;
    }

    LRESULT WTFWindow::DefaultWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        return WndProc(hWnd, message, wParam, lParam);
    }

    LRESULT CALLBACK WTFWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        WTFWindow* window = nullptr;

        if (WM_NCCREATE == message) {
            LPCREATESTRUCT cs = reinterpret_cast<LPCREATESTRUCT>(lParam);
            window = static_cast<WTFWindow*>(cs->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        } else {
            window = reinterpret_cast<WTFWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }

        if (window == nullptr)
            return DefWindowProc(hWnd, message, wParam, lParam);

        switch (message) {
            case WM_CREATE: {
                window->m_hWindow = hWnd;
                break;
            }
            case WM_DESTROY:
                PostQuitMessage(0);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }

        return 0;
    }

}

#endif // !_WTF_BUILD_UWP