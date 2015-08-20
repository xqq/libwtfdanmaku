#ifndef _WTF_WINDOW_HPP
#define _WTF_WINDOW_HPP

#include <Windows.h>
#include <cstdint>
#include "WTF_API.h"

namespace WTFDanmaku {

    class WTF_API WTFWindow {
    public:
        explicit WTFWindow(HINSTANCE hInst, int nCmdShow);
        ~WTFWindow();
        int Initialize(DWORD dwExStyle = NULL, int width = 1280, int height = 720, const wchar_t* title = nullptr);
        void SetCustomWindowProc(WNDPROC proc);
        void SetHitTestOverEnabled(bool enabled);
        int Run();
        HWND GetHwnd();
        LRESULT DefaultWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    private:
        ATOM RegisterWindowClass();
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    private:
        WTFWindow(const WTFWindow&) = delete;
        WTFWindow& operator=(const WTFWindow&) = delete;
    private:
        int m_nCmdShow = 0;
        HINSTANCE m_hInstance = NULL;
        WNDPROC m_CustomWndProc = NULL;
        HWND m_hWindow = NULL;
        PWSTR m_WindowClassName = L"WTFDanmaku";

        uint32_t m_ClientWidth = 0;
        uint32_t m_ClientHeight = 0;
    };

}

#endif // _WTF_SAMPLE_WINDOW_HPP