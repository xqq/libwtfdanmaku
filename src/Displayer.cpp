#include "DisplayerImpl.hpp"
#include "Displayer.hpp"

namespace WTFDanmaku {

    Displayer::Displayer() : pImpl(new DisplayerImpl(this)) { }

    Displayer::~Displayer() {
        pImpl.reset();
    }

    void Displayer::SetTarget(void* windowHandle) {
        pImpl->SetTarget(static_cast<HWND>(windowHandle));
    }

    bool Displayer::SetupBackend() {
        return pImpl->SetupBackend();
    }

    bool Displayer::TeardownBackend() {
        return pImpl->TeardownBackend();
    }

    int Displayer::GetWidth() {
        return pImpl->GetWidth();
    }

    int Displayer::GetHeight() {
        return pImpl->GetHeight();
    }

    float Displayer::GetDpiX() {
        return pImpl->GetDpiX();
    }

    float Displayer::GetDpiY() {
        return pImpl->GetDpiY();
    }

    void Displayer::Resize(int width, int height) {
        pImpl->Resize(width, height);
    }

    ComPtr<ID2D1Bitmap1> Displayer::CreateBitmap(float width, float height) {
        return pImpl->CreateBitmap(width, height);
    }

    ComPtr<ID2D1RenderTarget> Displayer::ObtainRenderTarget(ComPtr<ID2D1Bitmap1> bitmap) {
        return pImpl->ObtainRenderTarget(bitmap);
    }

    ComPtr<IDWriteFactory> Displayer::GetDWriteFactory() {
        return pImpl->GetDWriteFactory();
    }

    void Displayer::DrawDanmakuItem(DanmakuRef item, time_t current) {
        pImpl->DrawDanmakuItem(item, current);
    }

    void Displayer::BeginDraw() {
        pImpl->BeginDraw();
    }

    HRESULT Displayer::EndDraw() {
        return pImpl->EndDraw();
    }

}