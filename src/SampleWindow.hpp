#ifndef _DCD_SAMPLE_WINDOW_HPP
#define _DCD_SAMPLE_WINDOW_HPP

#include <Windows.h>
#include <cstdint>
#include "Noncopyable.hpp"

namespace DCDanmaku {

    class SampleWindow : public Noncopyable {
    public:
        explicit SampleWindow(HINSTANCE hInst, int nCmdShow);
        ~SampleWindow();
        int Initialize(DWORD dwExStyle = NULL);
        void SetHitTestOverEnabled(bool enabled);
        int Run();
    private:

    private:
        int m_nCmdShow = 0;
        HINSTANCE m_hInstance = NULL;
        HWND m_hWindow = NULL;
        PWSTR m_WindowClassName = L"DCDanmamku";

        uint32_t m_ClientWidth = 0;
        uint32_t m_ClientHeight = 0;
    };

}

#endif // _DCD_SAMPLE_WINDOW_HPP