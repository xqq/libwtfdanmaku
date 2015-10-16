#include <string>
#include "Controller.hpp"
#include "BaseDanmaku.hpp"
#include "DanmakusManager.hpp"
#include "DanmakuFactory.hpp"
#include "BilibiliParser.hpp"
#include "../include/WTFEngine.hpp"

namespace WTFDanmaku {

    WTFEngine::WTFEngine() {
        mController = new Controller;
    }

    WTFEngine::~WTFEngine() {
        Controller::State state = mController->GetState();
        if (state == Controller::State::kRunning || state == Controller::State::kPaused) {
            mController->Stop();
        }
        delete mController;
    }

    int WTFEngine::Initialize(void* hwnd) {
        return mController->Initialize(hwnd);
    }

    int WTFEngine::InitializeOffscreen(uint32_t initialWidth, uint32_t initialHeight) {
        return mController->Initialize(NULL, initialWidth, initialHeight);
    }

    void WTFEngine::Terminate() {
        mController->Terminate();
    }

    int WTFEngine::QuerySwapChain(const void* pGuid, void** ppObject) {
        return mController->QuerySwapChain(pGuid, ppObject);
    }

    void WTFEngine::LoadBilibiliFile(const char* filePath) {
        ParserRef parser = BilibiliParser::Create();
        parser->ParseFileSource(filePath);
        mController->GetManager()->SetDanmakuList(std::move(parser->GetDanmakus()));
    }

    void WTFEngine::LoadBilibiliFile(const wchar_t* filePath) {
        ParserRef parser = BilibiliParser::Create();
        parser->ParseFileSource(filePath);
        mController->GetManager()->SetDanmakuList(std::move(parser->GetDanmakus()));
    }

    void WTFEngine::LoadBilibiliXml(const char* str) {
        ParserRef parser = BilibiliParser::Create();
        parser->ParseStringSource(str);
        mController->GetManager()->SetDanmakuList(std::move(parser->GetDanmakus()));
    }

    void WTFEngine::AddDanmaku(Type type, time_t time, const wchar_t* comment, int fontSize, int fontColor, time_t timestamp, int danmakuId) {
        DanmakuRef danmaku = DanmakuFactory::CreateDanmaku(static_cast<DanmakuType>(type), time, std::wstring(comment), fontSize, fontColor, timestamp, danmakuId);
        mController->GetManager()->AddDanmaku(danmaku);
    }

    void WTFEngine::AddLiveDanmaku(Type type, time_t time, const wchar_t* comment, int fontSize, int fontColor, time_t timestamp, int danmakuId) {
        DanmakuRef danmaku = DanmakuFactory::CreateDanmaku(static_cast<DanmakuType>(type), time, std::wstring(comment), fontSize, fontColor, timestamp, danmakuId);
        mController->GetManager()->AddLiveDanmaku(danmaku);
    }

    void WTFEngine::Start() {
        mController->Start();
    }

    void WTFEngine::Pause() {
        mController->Pause();
    }

    void WTFEngine::Resume() {
        mController->Resume();
    }

    void WTFEngine::Stop() {
        mController->Stop();
    }

    void WTFEngine::SeekTo(time_t milliseconds) {
        mController->SeekTo(milliseconds);
    }

    void WTFEngine::Resize(uint32_t width, uint32_t height) {
        mController->Resize(width, height);
    }

    void WTFEngine::SetDpi(uint32_t dpiX, uint32_t dpiY) {
        mController->SetDpi(dpiX, dpiY);
    }

    time_t WTFEngine::GetCurrentPosition() {
        return mController->GetCurrentPosition();
    }

    bool WTFEngine::IsRunning() {
        return mController->IsRunning();
    }

    float WTFEngine::GetFontScaleFactor() {
        return mController->GetManager()->GetConfig()->FontScaleFactor;
    }

    void WTFEngine::SetFontScaleFactor(float factor) {
        mController->GetManager()->GetConfig()->FontScaleFactor = factor;
        mController->ReLayout();
    }

    void WTFEngine::SetFontName(const wchar_t* fontName) {
        mController->GetManager()->GetConfig()->FontName = std::wstring(fontName);
    }

    void WTFEngine::SetFontWeight(int dwriteFontWeight) {
        mController->GetManager()->GetConfig()->FontWeight = static_cast<DWRITE_FONT_WEIGHT>(dwriteFontWeight);
    }

    void WTFEngine::SetFontStyle(int dwriteFontStyle) {
        mController->GetManager()->GetConfig()->FontStyle = static_cast<DWRITE_FONT_STYLE>(dwriteFontStyle);
    }

    void WTFEngine::SetFontStretch(int dwriteFontStretch) {
        mController->GetManager()->GetConfig()->FontStretch = static_cast<DWRITE_FONT_STRETCH>(dwriteFontStretch);
    }

    void WTFEngine::SetDanmakuStyle(Style style) {
        mController->GetManager()->GetConfig()->DanmakuStyle = static_cast<DanmakuStyle>(style);
    }

    void WTFEngine::SetCompositionOpacity(float opacity) {
        float value = opacity > 1.0f ? 1.0f : (opacity < 0.0f ? 0.0f : opacity);
        mController->GetManager()->GetConfig()->CompositionOpacity = value;
    }

    void WTFEngine::SetDanmakuTypeVisibility(int params) {
        DanmakuConfig* config = mController->GetManager()->GetConfig();

        config->R2LVisible = (params & ScrollingVisible) ? true : false;
        config->TopVisible = (params & TopVisible) ? true : false;
        config->BottomVisible = (params & BottomVisible) ? true : false;
    }

}
