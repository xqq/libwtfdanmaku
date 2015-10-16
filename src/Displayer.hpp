#ifndef _WTF_DISPLAYER_HPP
#define _WTF_DISPLAYER_HPP

#include <wrl.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <cstdint>
#include <memory>
#include "BaseDanmaku.hpp"
#include "Noncopyable.hpp"

using Microsoft::WRL::ComPtr;

namespace WTFDanmaku {

    class DisplayerImpl;
    struct DanmakuConfig;

    class Displayer : public Noncopyable {
    public:
        explicit Displayer();
        ~Displayer();
        void SetTarget(void* windowHandle, uint32_t initialWidth = 0, uint32_t initialHeight = 0);
        bool SetupBackend();
        bool TeardownBackend();
        int QuerySwapChain(const void* pGuid, void** ppObject);
        int GetWidth();
        int GetHeight();
        float GetDpiX();
        float GetDpiY();
        void Resize(uint32_t width, uint32_t height);
        void SetDpi(uint32_t dpiX, uint32_t dpiY);
        ComPtr<ID2D1Bitmap1> CreateBitmap(uint32_t width, uint32_t height);
        ComPtr<ID2D1DeviceContext> AcquireDeviceContext(ComPtr<ID2D1Bitmap1> bitmap);
        void ReleaseDeviceContext(ComPtr<ID2D1DeviceContext> deviceContext);
        void DrawDanmakuItem(DanmakuRef item, time_t current, DanmakuConfig* config);
        void BeginDraw();
        HRESULT EndDraw();
        HRESULT Present();
        ComPtr<ID2D1Factory1> GetD2DFactory();
        ComPtr<IDWriteFactory> GetDWriteFactory();
    private:
        std::unique_ptr<DisplayerImpl> pImpl;
    };

}

#endif // _WTF_DISPLAYER_HPP