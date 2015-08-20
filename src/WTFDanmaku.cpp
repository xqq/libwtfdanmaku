#include "Controller.hpp"
#include "BaseDanmaku.hpp"
#include "DanmakusManager.hpp"
#include "DanmakuFactory.hpp"
#include "BilibiliParser.hpp"
#include "../include/WTFDanmaku.h"

using namespace WTFDanmaku;

WTF_C_API void*    __stdcall WTF_CreateInstance() {
    WTFInstance* instance = new WTFInstance;
    instance->controller = new Controller;
    return instance;
}

WTF_C_API void     __stdcall WTF_ReleaseInstance(WTFInstance* instance) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    Controller::State state = controller->GetState();
    if (state == Controller::State::kRunning || state == Controller::State::kPaused) {
        controller->Stop();
    }
    delete controller;
    delete instance;
}

WTF_C_API void     __stdcall WTF_InitializeWithHwnd(WTFInstance* instance, void* hwnd) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    controller->Initialize(hwnd);
}

WTF_C_API void     __stdcall WTF_LoadBilibiliFile(WTFInstance* instance, const char* filePath) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    ParserRef parser = BilibiliParser::Create();
    parser->ParseFileSource(filePath);
    controller->GetManager()->SetDanmakuList(std::move(parser->GetDanmakus()));
}

WTF_C_API void     __stdcall WTF_LoadBilibiliXml(WTFInstance* instance, const char* str) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    ParserRef parser = BilibiliParser::Create();
    parser->ParseStringSource(str);
    controller->GetManager()->SetDanmakuList(std::move(parser->GetDanmakus()));
}

WTF_C_API void     __stdcall WTF_AddDanmaku(WTFInstance* instance, int type, int64_t time, const wchar_t* comment, int fontSize, int fontColor, int64_t timestamp, int danmakuId) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    DanmakuRef danmaku = DanmakuFactory::CreateDanmaku(static_cast<DanmakuType>(type), time, std::wstring(comment), fontSize, fontColor, timestamp, danmakuId);
    controller->GetManager()->AddDanmaku(danmaku);
}

WTF_C_API void     __stdcall WTF_AddLiveDanmaku(WTFInstance* instance, int type, int64_t time, const wchar_t* comment, int fontSize, int fontColor, int64_t timestamp, int danmakuId) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    DanmakuRef danmaku = DanmakuFactory::CreateDanmaku(static_cast<DanmakuType>(type), time, std::wstring(comment), fontSize, fontColor, timestamp, danmakuId);
    controller->GetManager()->AddLiveDanmaku(danmaku);
}

WTF_C_API void     __stdcall WTF_Start(WTFInstance* instance) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    controller->Start();
}

WTF_C_API void     __stdcall WTF_Pause(WTFInstance* instance) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    controller->Pause();
}

WTF_C_API void     __stdcall WTF_Resume(WTFInstance* instance) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    controller->Resume();
}

WTF_C_API void     __stdcall WTF_Stop(WTFInstance* instance) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    controller->Stop();
}

WTF_C_API void     __stdcall WTF_SeekTo(WTFInstance* instance, int64_t milliseconds) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    controller->SeekTo(milliseconds);
}

WTF_C_API int64_t  __stdcall WTF_GetCurrentPosition(WTFInstance* instance) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    return controller->GetCurrentPosition();
}

WTF_C_API int      __stdcall WTF_IsRunning(WTFInstance* instance) {
    Controller* controller = reinterpret_cast<Controller*>(instance->controller);

    return static_cast<int>(controller->IsRunning());
}
