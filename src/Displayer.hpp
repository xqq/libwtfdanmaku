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
        ComPtr<ID2D1Bitmap1> CreateBitmap(uint32_t width, uint32_t height);
        ComPtr<ID2D1RenderTarget> AcquireRenderTarget(ComPtr<ID2D1Bitmap1> bitmap);
        void ReleaseRenderTarget(ComPtr<ID2D1RenderTarget> renderTarget);
        void DrawDanmakuItem(DanmakuRef item, time_t current, DanmakuConfig* config);
        void BeginDraw();
        HRESULT EndDraw();
        ComPtr<ID2D1Factory1> GetD2DFactory();
        ComPtr<IDWriteFactory> GetDWriteFactory();
    private:
        std::unique_ptr<DisplayerImpl> pImpl;
    };

}

#endif // _WTF_DISPLAYER_HPP