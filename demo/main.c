#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "WTFDanmaku.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

bool hasArgFile = false;
char optFileName[MAX_PATH] = { 0 };
bool wtfInited = false;
WTF_Window* window = NULL;
WTF_Instance* wtf = NULL;

int main(int argc, char** argv) {
    if (argv[1]) {
        if (strlen(argv[1]) > 0) {
            strcpy_s(optFileName, strlen(argv[1]), argv[1]);
            hasArgFile = true;
        }
    }
    window = WTFWindow_Create(GetModuleHandle(NULL), SW_SHOWNORMAL);
    WTFWindow_SetHitTestOverEnabled(window, 1);
    WTFWindow_SetCustomWndProc(window, WndProc);
    WTFWindow_Initialize(window, WS_EX_NOREDIRECTIONBITMAP, 1280, 720, L"WTFDanmaku Demo");
    int ret = WTFWindow_RunMessageLoop(window);
    WTFWindow_Release(window);
    return ret;
}

void InitializeWTF(HWND hwnd) {
    if (!wtfInited) {
        wtf = WTF_CreateInstance();
        WTF_InitializeWithHwnd(wtf, (void*)hwnd);
        if (hasArgFile) {
            WTF_LoadBilibiliFile(wtf, optFileName);
        } else {
            WTF_LoadBilibiliFile(wtf, "F:\\Downloads\\Downloads\\1055660.xml");
        }
        wtfInited = true;
    }
    WTF_Start(wtf);
}

void RButtonWTF(HWND hwnd) {
    if (wtf) {
        WTF_Pause(wtf);
    }
}

void ReleaseWTF(HWND hwnd) {
    if (wtf) {
        if (WTF_IsRunning(wtf)) {
            WTF_Stop(wtf);
        }
        WTF_ReleaseInstance(wtf);
        wtf = NULL;
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_LBUTTONDOWN:
            InitializeWTF(hwnd);
            break;
        case WM_RBUTTONDOWN:
            RButtonWTF(hwnd);
            break;
        case WM_DESTROY:
            ReleaseWTF(hwnd);
    }
    return WTFWindow_DefaultWindowProc(window, (void*)hwnd, message, (WPARAM)wParam, (LPARAM)lParam);
}
