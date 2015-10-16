#ifndef _WTF_DANMAKU_C_WRAPPER_H
#define _WTF_DANMAKU_C_WRAPPER_H

#include <stdint.h>
#include <minwindef.h>
#include "WTF_API.h"

WTF_C_API typedef struct WTF_Instance {
    void* controller;
} WTF_Instance;

#define WTF_DANMAKU_TYPE_SCROLLING 1
#define WTF_DANMAKU_TYPE_BOTTOM    4
#define WTF_DANMAKU_TYPE_TOP       5
#define WTF_DANMAKU_TYPE_RESERVE   6
#define WTF_DANMAKU_TYPE_POSITION  7
#define WTF_DANMAKU_TYPE_ADVANCED  8

#define WTF_DANMAKU_STYLE_OUTLINE    1
#define WTF_DANMAKU_STYLE_PROJECTION 2

#define WTF_DANMAKU_TYPE_SCROLLING_VISIBLE (1)
#define WTF_DANMAKU_TYPE_BOTTOM_VISIBLE    (2)
#define WTF_DANMAKU_TYPE_TOP_VISIBLE       (4)
#define WTF_DANMAKU_TYPE_RESERVE_VISIBLE   (8)
#define WTF_DANMAKU_TYPE_POSITION_VISIBLE  (16)
#define WTF_DANMAKU_TYPE_ADVANCED_VISIBLE  (32)

WTF_C_API WTF_Instance* __stdcall WTF_CreateInstance();
WTF_C_API void     __stdcall WTF_ReleaseInstance(WTF_Instance* instance);
WTF_C_API int      __stdcall WTF_InitializeWithHwnd(WTF_Instance* instance, void* hwnd);
WTF_C_API int      __stdcall WTF_InitializeOffscreen(WTF_Instance* instance, uint32_t initialWidth, uint32_t initialHeight);
WTF_C_API void     __stdcall WTF_Terminate(WTF_Instance* instance);
WTF_C_API int      __stdcall WTF_QuerySwapChain(WTF_Instance* instance, const void* pGuid, void** ppObject);
WTF_C_API void     __stdcall WTF_LoadBilibiliFile(WTF_Instance* instance, const char* filePath);
WTF_C_API void     __stdcall WTF_LoadBilibiliFileW(WTF_Instance* instance, const wchar_t* filePath);
WTF_C_API void     __stdcall WTF_LoadBilibiliXml(WTF_Instance* instance, const char* str);
WTF_C_API void     __stdcall WTF_AddDanmaku(WTF_Instance* instance, int type, int64_t time, const wchar_t* comment, int fontSize, int fontColor, int64_t timestamp, int danmakuId);
WTF_C_API void     __stdcall WTF_AddLiveDanmaku(WTF_Instance* instance, int type, int64_t time, const wchar_t* comment, int fontSize, int fontColor, int64_t timestamp, int danmakuId);
WTF_C_API void     __stdcall WTF_Start(WTF_Instance* instance);
WTF_C_API void     __stdcall WTF_Pause(WTF_Instance* instance);
WTF_C_API void     __stdcall WTF_Resume(WTF_Instance* instance);
WTF_C_API void     __stdcall WTF_Stop(WTF_Instance* instance);
WTF_C_API void     __stdcall WTF_SeekTo(WTF_Instance* instance, int64_t milliseconds);
WTF_C_API void     __stdcall WTF_Resize(WTF_Instance* instance, uint32_t width, uint32_t height);
WTF_C_API void     __stdcall WTF_SetDpi(WTF_Instance* instance, uint32_t dpiX, uint32_t dpiY);
WTF_C_API int64_t  __stdcall WTF_GetCurrentPosition(WTF_Instance* instance);
WTF_C_API int      __stdcall WTF_IsRunning(WTF_Instance* instance);
WTF_C_API float    __stdcall WTF_GetFontScaleFactor(WTF_Instance* instance);
WTF_C_API void     __stdcall WTF_SetFontScaleFactor(WTF_Instance* instance, float factor);
WTF_C_API void     __stdcall WTF_SetFontName(WTF_Instance* instance, const wchar_t* fontName);
WTF_C_API void     __stdcall WTF_SetFontWeight(WTF_Instance* instance, int dwriteFontWeight);
WTF_C_API void     __stdcall WTF_SetFontStyle(WTF_Instance* instance, int dwriteFontStyle);
WTF_C_API void     __stdcall WTF_SetFontStretch(WTF_Instance* instance, int dwriteFontStretch);
WTF_C_API void     __stdcall WTF_SetDanmakuStyle(WTF_Instance* instance, int style);
WTF_C_API void     __stdcall WTF_SetCompositionOpacity(WTF_Instance* instance, float opacity);
WTF_C_API void     __stdcall WTF_SetDanmakuTypeVisibility(WTF_Instance* instance, int params);

#ifndef _WTF_BUILD_UWP

WTF_C_API typedef struct WTF_Window WTF_Window;

WTF_C_API WTF_Window* __stdcall WTFWindow_Create(void* hInstance, int nCmdShow);
WTF_C_API void        __stdcall WTFWindow_Release(WTF_Window* window);
WTF_C_API void        __stdcall WTFWindow_Initialize(WTF_Window* window, uint32_t dwExStyle, int width, int height, const wchar_t* title);
WTF_C_API void*       __stdcall WTFWindow_GetHwnd(WTF_Window* window);
WTF_C_API void        __stdcall WTFWindow_SetCustomWndProc(WTF_Window* window, void* proc);
WTF_C_API LRESULT     __stdcall WTFWindow_DefaultWindowProc(WTF_Window* window, void* hwnd, uint32_t message, WPARAM wParam, LPARAM lParam);
WTF_C_API void        __stdcall WTFWindow_SetHitTestOverEnabled(WTF_Window* window, int enabled);
WTF_C_API int         __stdcall WTFWindow_RunMessageLoop(WTF_Window* window);

#endif // !_WTF_BUILD_UWP


#endif // _WTF_DANMAKU_C_WRAPPER_H