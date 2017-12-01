#include "DisplayerImpl.hpp"
#include "Displayer.hpp"

namespace WTFDanmaku {

    Displayer::Displayer() : pImpl(new DisplayerImpl(this)) { }

    Displayer::~Displayer() {
        pImpl.reset();
    }

    void Displayer::SetTarget(void* windowHandle, uint32_t initialWidth, uint32_t initialHeight) {
        pImpl->SetTarget(static_cast<HWND>(windowHandle), initialWidth, initialHeight);
    }

    bool Displayer::SetupBackend() {
        return pImpl->SetupBackend();
    }

    bool Displayer::TeardownBackend() {
        return pImpl->TeardownBackend();
    }

    int Displayer::QuerySwapChain(const void* pGuid, void** ppObject) {
        return pImpl->QuerySwapChain(reinterpret_cast<const IID*>(pGuid), ppObject);
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

    void Displayer::Resize(uint32_t width, uint32_t height) {
        pImpl->Resize(width, height);
    }

    void Displayer::SetDpi(uint32_t dpiX, uint32_t dpiY) {
        pImpl->SetDpi(dpiX, dpiY);
    }

    ComPtr<ID2D1Bitmap1> Displayer::CreateBitmap(uint32_t width, uint32_t height) {
        return pImpl->CreateBitmap(width, height);
    }

    ComPtr<ID2D1DeviceContext> Displayer::AcquireDeviceContext(ComPtr<ID2D1Bitmap1> bitmap) {
        return pImpl->AcquireDeviceContext(bitmap);
    }

    void Displayer::ReleaseDeviceContext(ComPtr<ID2D1DeviceContext> deviceContext) {
        pImpl->ReleaseDeviceContext(deviceContext);
    }

    ComPtr<ID2D1Factory1> Displayer::GetD2DFactory() {
        return pImpl->GetD2DFactory();
    }

    ComPtr<IDWriteFactory> Displayer::GetDWriteFactory() {
        return pImpl->GetDWriteFactory();
    }

    void Displayer::DrawDanmakuItem(DanmakuRef item, time_t current, DanmakuConfig* config) {
        pImpl->DrawDanmakuItem(item, current, config);
    }

    void Displayer::BeginDraw() {
        pImpl->BeginDraw();
    }

    HRESULT Displayer::EndDraw() {
        return pImpl->EndDraw();
    }

    HRESULT Displayer::Present() {
        return pImpl->Present();
    }

}