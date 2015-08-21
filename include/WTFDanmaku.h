#ifndef _WTF_DANMAKU_C_WRAPPER_H
#define _WTF_DANMAKU_C_WRAPPER_H

#include <stdint.h>
#include "WTF_API.h"

WTF_C_API typedef struct WTF_Instance {
    void* controller;
} WTF_Instance;

WTF_C_API WTF_Instance* __stdcall WTF_CreateInstance();
WTF_C_API void     __stdcall WTF_ReleaseInstance(WTF_Instance* instance);
WTF_C_API void     __stdcall WTF_InitializeWithHwnd(WTF_Instance* instance, void* hwnd);
WTF_C_API void     __stdcall WTF_LoadBilibiliFile(WTF_Instance* instance, const char* filePath);
WTF_C_API void     __stdcall WTF_LoadBilibiliXml(WTF_Instance* instance, const char* str);
WTF_C_API void     __stdcall WTF_AddDanmaku(WTF_Instance* instance, int type, int64_t time, const wchar_t* comment, int fontSize, int fontColor, int64_t timestamp, int danmakuId);
WTF_C_API void     __stdcall WTF_AddLiveDanmaku(WTF_Instance* instance, int type, int64_t time, const wchar_t* comment, int fontSize, int fontColor, int64_t timestamp, int danmakuId);
WTF_C_API void     __stdcall WTF_Start(WTF_Instance* instance);
WTF_C_API void     __stdcall WTF_Pause(WTF_Instance* instance);
WTF_C_API void     __stdcall WTF_Resume(WTF_Instance* instance);
WTF_C_API void     __stdcall WTF_Stop(WTF_Instance* instance);
WTF_C_API void     __stdcall WTF_SeekTo(WTF_Instance* instance, int64_t milliseconds);
WTF_C_API void     __stdcall WTF_Resize(WTF_Instance* instance, uint32_t width, uint32_t height);
WTF_C_API int64_t  __stdcall WTF_GetCurrentPosition(WTF_Instance* instance);
WTF_C_API int      __stdcall WTF_IsRunning(WTF_Instance* instance);

WTF_C_API typedef struct WTF_Window WTF_Window;

WTF_C_API WTF_Window* __stdcall WTFWindow_Create(void* hInstance, int nCmdShow);
WTF_C_API void        __stdcall WTFWindow_Release(WTF_Window* window);
WTF_C_API void        __stdcall WTFWindow_Initialize(WTF_Window* window, uint32_t dwExStyle, int width, int height, const wchar_t* title);
WTF_C_API void*       __stdcall WTFWindow_GetHwnd(WTF_Window* window);
WTF_C_API void        __stdcall WTFWindow_SetCustomWndProc(WTF_Window* window, void* proc);
WTF_C_API int         __stdcall WTFWindow_DefaultWindowProc(WTF_Window* window, void* hwnd, uint32_t message, uint32_t wParam, uint32_t lParam);
WTF_C_API void        __stdcall WTFWindow_SetHitTestOverEnabled(WTF_Window* window, int enabled);
WTF_C_API int         __stdcall WTFWindow_RunMessageLoop(WTF_Window* window);


#endif // _WTF_DANMAKU_C_WRAPPER_H