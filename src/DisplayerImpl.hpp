#ifndef _WTF_DISPLAYER_IMPL_HPP
#define _WTF_DISPLAYER_IMPL_HPP

#include <Windows.h>
#include <wrl.h>
#include <d2d1_1.h>
#include <d3d10_1.h>
#include <dxgi1_3.h>
#include <dcomp.h>
#include <dwrite.h>
#include "Win32Mutex.hpp"
#include "Noncopyable.hpp"
#include "BaseDanmaku.hpp"

using Microsoft::WRL::ComPtr;

namespace WTFDanmaku {

    class Displayer;

    class DisplayerImpl : public Noncopyable {
    public:
        explicit DisplayerImpl(Displayer* outer);
        ~DisplayerImpl();
        void SetTarget(HWND windowHandle);
        bool SetupBackend();
        bool TeardownBackend();
        void Resize(int width, int height);
        ComPtr<ID2D1Bitmap1> CreateBitmap(float width, float height);
        ComPtr<ID2D1RenderTarget> ObtainRenderTarget(ComPtr<ID2D1Bitmap1> bitmap);
        void DrawDanmakuItem(DanmakuRef item, time_t current);
        void BeginDraw();
        HRESULT EndDraw();
        ComPtr<IDWriteFactory> GetDWriteFactory();
    public:
        inline int GetWidth() {
            return mWidth;
        }

        inline int GetHeight() {
            return mHeight;
        }

        inline float GetDpiX() {
            return mDpiX;
        }

        inline float GetDpiY() {
            return mDpiY;
        }
    private:
        HWND mHwnd = 0;
        int mWidth = 0;
        int mHeight = 0;
        float mDpiX = 0.0f;
        float mDpiY = 0.0f;
        bool mHasBackend = false;
        Displayer* mOuter = nullptr;
        Win32Mutex mRenderMutex;
        bool mInRendering = false;
        ComPtr<ID3D10Device1> mD3DDevice;
        ComPtr<IDXGIFactory2> mDxgiFactory;
        ComPtr<IDXGIDevice> mDxgiDevice;
        ComPtr<IDXGISurface> mDxgiSurface;
        ComPtr<IDXGISwapChain1> mSwapChain;
        ComPtr<IDWriteFactory> mDWriteFactory;
        ComPtr<ID2D1Factory1> mD2DFactory;
        ComPtr<ID2D1Device> mD2DDevice;
        ComPtr<ID2D1DeviceContext> mDeviceContext;
        //ComPtr<ID2D1RenderTarget> mRenderTarget;
        ComPtr<IDCompositionDevice> mDCompDevice;
        ComPtr<IDCompositionTarget> mDCompTarget;
        ComPtr<IDCompositionVisual> mDCompVisual;
    };

}

#endif // _WTF_DISPLAYER_IMPL_HPP