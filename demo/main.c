#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>
#include "WTFDanmaku.h"

#ifndef WS_EX_NOREDIRECTIONBITMAP
    #define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#endif

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

bool hasArgFile = false;
char optFileName[MAX_PATH] = { 0 };
bool wtfInited = false;
WTF_Window* window = NULL;
WTF_Instance* wtf = NULL;

int main(int argc, char** argv) {
    if (argv[1]) {
        if (strlen(argv[1]) > 0) {
            strcpy_s(optFileName, sizeof(optFileName), argv[1]);
            hasArgFile = true;
        }
    }
    window = WTFWindow_Create(GetModuleHandle(NULL), SW_SHOWNORMAL);
    WTFWindow_SetCustomWndProc(window, WndProc);
    WTFWindow_Initialize(window, WS_EX_NOREDIRECTIONBITMAP, 1280, 720, L"WTFDanmaku Demo");
    WTFWindow_SetHitTestOverEnabled(window, 0);
    int ret = WTFWindow_RunMessageLoop(window);
    WTFWindow_Release(window);
    return ret;
}

void InitializeWTF(HWND hwnd) {
    if (!wtfInited) {
        wtf = WTF_CreateInstance();
        WTF_InitializeWithHwnd(wtf, (void*)hwnd);
        WTF_SetFontName(wtf, L"SimHei");
        WTF_SetFontWeight(wtf, 700);
        WTF_SetFontScaleFactor(wtf, 1.0f);
        WTF_SetDanmakuStyle(wtf, WTF_DANMAKU_STYLE_OUTLINE);
        WTF_SetCompositionOpacity(wtf, 0.9f);
        WTF_SetDanmakuTypeVisibility(wtf, WTF_DANMAKU_TYPE_SCROLLING_VISIBLE |
                                          WTF_DANMAKU_TYPE_TOP_VISIBLE | 
                                          WTF_DANMAKU_TYPE_BOTTOM_VISIBLE);

        if (hasArgFile) {
            WTF_LoadBilibiliFile(wtf, optFileName);
        } else {
            WTF_LoadBilibiliFile(wtf, "F:\\Downloads\\Downloads\\5cmps.xml");
        }
        wtfInited = true;
    }
    WTF_Start(wtf);
}

void ResizeWTF(uint32_t width, uint32_t height) {
    if (wtf) {
        WTF_Resize(wtf, width, height);
    }
}

void RButtonWTF() {
    if (wtf) {
        WTF_Pause(wtf);
    }
}

void ReleaseWTF() {
    if (wtf) {
        if (WTF_IsRunning(wtf)) {
            WTF_Stop(wtf);
        }
        WTF_ReleaseInstance(wtf);
        wtf = NULL;
        wtfInited = false;
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_SIZE:
            if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED) {
                ResizeWTF(LOWORD(lParam), HIWORD(lParam));
            }
            break;
        case WM_LBUTTONDOWN:
            InitializeWTF(hwnd);
            break;
        case WM_RBUTTONDOWN:
            RButtonWTF();
            break;
        case WM_DESTROY:
            ReleaseWTF();
            break;
        case WM_DPICHANGED: {
            uint32_t dpi = LOWORD(wParam);
            if (wtf != NULL && WTF_IsRunning(wtf)) {
                WTF_SetDpi(wtf, dpi, dpi);
            }
            break;
        }
    }
    return WTFWindow_DefaultWindowProc(window, (void*)hwnd, message, (WPARAM)wParam, (LPARAM)lParam);
}
