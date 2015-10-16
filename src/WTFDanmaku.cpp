#include "Controller.hpp"
#include "BaseDanmaku.hpp"
#include "DanmakusManager.hpp"
#include "DanmakuConfig.hpp"
#include "DanmakuFactory.hpp"
#include "BilibiliParser.hpp"
#include "../include/WTFWindow.hpp"
#include "../include/WTFDanmaku.h"

using namespace WTFDanmaku;

WTF_C_API WTF_Instance* __stdcall WTF_CreateInstance() {
    WTF_Instance* instance = new WTF_Instance;
    instance->controller = new Controller;
    return instance;
}

WTF_C_API void     __stdcall WTF_ReleaseInstance(WTF_Instance* instance) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    Controller::State state = controller->GetState();
    if (state == Controller::State::kRunning || state == Controller::State::kPaused) {
        controller->Stop();
    }
    delete controller;
    delete instance;
}

WTF_C_API int      __stdcall WTF_InitializeWithHwnd(WTF_Instance* instance, void* hwnd) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    return controller->Initialize(hwnd);
}

WTF_C_API int      __stdcall WTF_InitializeOffscreen(WTF_Instance* instance, uint32_t initialWidth, uint32_t initialHeight) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    return controller->Initialize(NULL, initialWidth, initialHeight);
}

WTF_C_API void     __stdcall WTF_Terminate(WTF_Instance* instance) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    controller->Terminate();
}

WTF_C_API int      __stdcall WTF_QuerySwapChain(WTF_Instance* instance, const void* pGuid, void** ppObject) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    return controller->QuerySwapChain(pGuid, ppObject);
}

WTF_C_API void     __stdcall WTF_LoadBilibiliFile(WTF_Instance* instance, const char* filePath) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    ParserRef parser = BilibiliParser::Create();
    parser->ParseFileSource(filePath);
    controller->GetManager()->SetDanmakuList(std::move(parser->GetDanmakus()));
}

WTF_C_API void     __stdcall WTF_LoadBilibiliFileW(WTF_Instance* instance, const wchar_t* filePath) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    ParserRef parser = BilibiliParser::Create();
    parser->ParseFileSource(filePath);
    controller->GetManager()->SetDanmakuList(std::move(parser->GetDanmakus()));
}

WTF_C_API void     __stdcall WTF_LoadBilibiliXml(WTF_Instance* instance, const char* str) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    ParserRef parser = BilibiliParser::Create();
    parser->ParseStringSource(str);
    controller->GetManager()->SetDanmakuList(std::move(parser->GetDanmakus()));
}

WTF_C_API void     __stdcall WTF_AddDanmaku(WTF_Instance* instance, int type, int64_t time, const wchar_t* comment, int fontSize, int fontColor, int64_t timestamp, int danmakuId) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    DanmakuRef danmaku = DanmakuFactory::CreateDanmaku(static_cast<DanmakuType>(type), time, std::wstring(comment), fontSize, fontColor, timestamp, danmakuId);
    controller->GetManager()->AddDanmaku(danmaku);
}

WTF_C_API void     __stdcall WTF_AddLiveDanmaku(WTF_Instance* instance, int type, int64_t time, const wchar_t* comment, int fontSize, int fontColor, int64_t timestamp, int danmakuId) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    DanmakuRef danmaku = DanmakuFactory::CreateDanmaku(static_cast<DanmakuType>(type), time, std::wstring(comment), fontSize, fontColor, timestamp, danmakuId);
    controller->GetManager()->AddLiveDanmaku(danmaku);
}

WTF_C_API void     __stdcall WTF_Start(WTF_Instance* instance) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    controller->Start();
}

WTF_C_API void     __stdcall WTF_Pause(WTF_Instance* instance) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    controller->Pause();
}

WTF_C_API void     __stdcall WTF_Resume(WTF_Instance* instance) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    controller->Resume();
}

WTF_C_API void     __stdcall WTF_Stop(WTF_Instance* instance) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    controller->Stop();
}

WTF_C_API void     __stdcall WTF_SeekTo(WTF_Instance* instance, int64_t milliseconds) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    controller->SeekTo(milliseconds);
}

WTF_C_API void     __stdcall WTF_Resize(WTF_Instance* instance, uint32_t width, uint32_t height) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    controller->Resize(width, height);
}

WTF_C_API void     __stdcall WTF_SetDpi(WTF_Instance* instance, uint32_t dpiX, uint32_t dpiY) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    controller->SetDpi(dpiX, dpiY);
}

WTF_C_API int64_t  __stdcall WTF_GetCurrentPosition(WTF_Instance* instance) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    return controller->GetCurrentPosition();
}

WTF_C_API int      __stdcall WTF_IsRunning(WTF_Instance* instance) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    return static_cast<int>(controller->IsRunning());
}

WTF_C_API float    __stdcall WTF_GetFontScaleFactor(WTF_Instance* instance) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);
    DanmakuConfig* config = controller->GetManager()->GetConfig();

    return config->FontScaleFactor;
}

WTF_C_API void     __stdcall WTF_SetFontScaleFactor(WTF_Instance* instance, float factor) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);
    DanmakuConfig* config = controller->GetManager()->GetConfig();

    config->FontScaleFactor = factor;
    controller->ReLayout();
}

WTF_C_API void     __stdcall WTF_SetFontName(WTF_Instance* instance, const wchar_t* fontName) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);
    DanmakuConfig* config = controller->GetManager()->GetConfig();

    config->FontName = std::wstring(fontName);
}

WTF_C_API void     __stdcall WTF_SetFontWeight(WTF_Instance* instance, int dwriteFontWeight) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);
    DanmakuConfig* config = controller->GetManager()->GetConfig();

    config->FontWeight = static_cast<DWRITE_FONT_WEIGHT>(dwriteFontWeight);
}

WTF_C_API void     __stdcall WTF_SetFontStyle(WTF_Instance* instance, int dwriteFontStyle) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);
    DanmakuConfig* config = controller->GetManager()->GetConfig();

    config->FontStyle = static_cast<DWRITE_FONT_STYLE>(dwriteFontStyle);
}

WTF_C_API void     __stdcall WTF_SetFontStretch(WTF_Instance* instance, int dwriteFontStretch) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);
    DanmakuConfig* config = controller->GetManager()->GetConfig();

    config->FontStretch = static_cast<DWRITE_FONT_STRETCH>(dwriteFontStretch);
}

WTF_C_API void     __stdcall WTF_SetDanmakuStyle(WTF_Instance* instance, int style) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);
    DanmakuConfig* config = controller->GetManager()->GetConfig();

    config->DanmakuStyle = static_cast<DanmakuStyle>(style);
}

WTF_C_API void     __stdcall WTF_SetCompositionOpacity(WTF_Instance* instance, float opacity) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);
    DanmakuConfig* config = controller->GetManager()->GetConfig();

    float value = opacity > 1.0f ? 1.0f : (opacity < 0.0f ? 0.0f : opacity);
    config->CompositionOpacity = value;
}

WTF_C_API void     __stdcall WTF_SetDanmakuTypeVisibility(WTF_Instance* instance, int params) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);
    DanmakuConfig* config = controller->GetManager()->GetConfig();

    config->R2LVisible = (params & WTF_DANMAKU_TYPE_SCROLLING_VISIBLE) ? true : false;
    config->TopVisible = (params & WTF_DANMAKU_TYPE_TOP_VISIBLE) ? true : false;
    config->BottomVisible = (params & WTF_DANMAKU_TYPE_BOTTOM_VISIBLE) ? true : false;
}

#ifndef _WTF_BUILD_UWP

WTF_C_API WTF_Window* __stdcall WTFWindow_Create(void* hInstance, int nCmdShow) {
    WTFWindow* wtfwindow = new WTFWindow(static_cast<HINSTANCE>(hInstance), nCmdShow);
    return reinterpret_cast<WTF_Window*>(wtfwindow);
}

WTF_C_API void       __stdcall WTFWindow_Release(WTF_Window* window) {
    WTFWindow* wtfwindow = reinterpret_cast<WTFWindow*>(window);

    delete wtfwindow;
}

WTF_C_API void       __stdcall WTFWindow_Initialize(WTF_Window* window, uint32_t dwExStyle, int width, int height, const wchar_t* title) {
    WTFWindow* wtfwindow = reinterpret_cast<WTFWindow*>(window);

    wtfwindow->Initialize(dwExStyle, width, height, title);
}

WTF_C_API void*      __stdcall WTFWindow_GetHwnd(WTF_Window* window) {
    WTFWindow* wtfwindow = reinterpret_cast<WTFWindow*>(window);

    return wtfwindow->GetHwnd();
}

WTF_C_API void       __stdcall WTFWindow_SetCustomWndProc(WTF_Window* window, void* proc) {
    WTFWindow* wtfwindow = reinterpret_cast<WTFWindow*>(window);

    wtfwindow->SetCustomWindowProc(reinterpret_cast<WNDPROC>(proc));
}

WTF_C_API LRESULT     __stdcall WTFWindow_DefaultWindowProc(WTF_Window* window, void* hwnd, uint32_t message, WPARAM wParam, LPARAM lParam) {
    WTFWindow* wtfwindow = reinterpret_cast<WTFWindow*>(window);

    return wtfwindow->DefaultWindowProc(static_cast<HWND>(hwnd), message, wParam, lParam);
}

WTF_C_API void       __stdcall WTFWindow_SetHitTestOverEnabled(WTF_Window* window, int enabled) {
    WTFWindow* wtfwindow = reinterpret_cast<WTFWindow*>(window);

    bool bEnabled = enabled == 0 ? false : true;
    wtfwindow->SetHitTestOverEnabled(bEnabled);
}

WTF_C_API int        __stdcall WTFWindow_RunMessageLoop(WTF_Window* window) {
    WTFWindow* wtfwindow = reinterpret_cast<WTFWindow*>(window);

    return wtfwindow->Run();
}

#endif // !_WTF_BUILD_UWP