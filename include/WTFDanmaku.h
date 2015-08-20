#ifndef _WTF_DANMAKU_C_WRAPPER_H
#define _WTF_DANMAKU_C_WRAPPER_H

#include <stdint.h>
#include "WTF_API.h"

WTF_C_API typedef struct WTFInstance {
    void* controller;
} WTFInstance;

WTF_C_API void*    __stdcall WTF_CreateInstance();
WTF_C_API void     __stdcall WTF_ReleaseInstance(WTFInstance* instance);
WTF_C_API void     __stdcall WTF_InitializeWithHwnd(WTFInstance* instance, void* hwnd);
WTF_C_API void     __stdcall WTF_LoadBilibiliFile(WTFInstance* instance, const char* filePath);
WTF_C_API void     __stdcall WTF_LoadBilibiliXml(WTFInstance* instance, const char* str);
WTF_C_API void     __stdcall WTF_AddDanmaku(WTFInstance* instance, int type, int64_t time, const wchar_t* comment, int fontSize, int fontColor, int64_t timestamp, int danmakuId);
WTF_C_API void     __stdcall WTF_AddLiveDanmaku(WTFInstance* instance, int type, int64_t time, const wchar_t* comment, int fontSize, int fontColor, int64_t timestamp, int danmakuId);
WTF_C_API void     __stdcall WTF_Start(WTFInstance* instance);
WTF_C_API void     __stdcall WTF_Pause(WTFInstance* instance);
WTF_C_API void     __stdcall WTF_Resume(WTFInstance* instance);
WTF_C_API void     __stdcall WTF_Stop(WTFInstance* instance);
WTF_C_API void     __stdcall WTF_SeekTo(WTFInstance* instance, int64_t milliseconds);
WTF_C_API int64_t  __stdcall WTF_GetCurrentPosition(WTFInstance* instance);
WTF_C_API int      __stdcall WTF_IsRunning(WTFInstance* instance);

#endif // _WTF_DANMAKU_C_WRAPPER_H