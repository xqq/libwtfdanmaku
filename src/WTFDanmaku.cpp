#include "Controller.hpp"
#include "BaseDanmaku.hpp"
#include "DanmakusManager.hpp"
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

WTF_C_API void     __stdcall WTF_InitializeWithHwnd(WTF_Instance* instance, void* hwnd) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    controller->Initialize(hwnd);
}

WTF_C_API void     __stdcall WTF_LoadBilibiliFile(WTF_Instance* instance, const char* filePath) {
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

WTF_C_API int64_t  __stdcall WTF_GetCurrentPosition(WTF_Instance* instance) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    return controller->GetCurrentPosition();
}

WTF_C_API int      __stdcall WTF_IsRunning(WTF_Instance* instance) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    return static_cast<int>(controller->IsRunning());
}


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

WTF_C_API int        __stdcall WTFWindow_DefaultWindowProc(WTF_Window* window, void* hwnd, uint32_t message, uint32_t wParam, uint32_t lParam) {
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