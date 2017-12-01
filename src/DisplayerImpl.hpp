#ifndef _WTF_DISPLAYER_IMPL_HPP
#define _WTF_DISPLAYER_IMPL_HPP

#include <Windows.h>
#include <wrl.h>
#include <d2d1_1.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#if !defined(_WTF_BUILD_WIN7) && !defined(_WTF_BUILD_UWP)
#include <dcomp.h>
#endif
#include <dwrite.h>
#include "Win32Mutex.hpp"
#include "Noncopyable.hpp"
#include "BaseDanmaku.hpp"

using Microsoft::WRL::ComPtr;

namespace WTFDanmaku {

    class Displayer;
    struct DanmakuConfig;

    class DisplayerImpl : public Noncopyable {
    public:
        explicit DisplayerImpl(Displayer* outer);
        ~DisplayerImpl();
        void SetTarget(HWND windowHandle, uint32_t initialWidth = 0, uint32_t initialHeight = 0);
        bool SetupBackend();
        bool TeardownBackend();
        HRESULT QuerySwapChain(const IID* pGuid, void** ppvObject);
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
        static HRESULT CreateD3D11Device(IDXGIAdapter* adapter, D3D_DRIVER_TYPE driverType, UINT flags,
                                         ID3D11Device** ppDevice, ID3D11DeviceContext** ppDevCtx, D3D_FEATURE_LEVEL* resultLevel);
        HRESULT CreateDeviceIndependentResources();
        HRESULT CreateDeviceResources();
        HRESULT CreateTargetDependentResources();
        HRESULT CreateDCompResources();
        HRESULT HandleDeviceLost();
    private:
        HWND mHwnd = 0;
        int mWidth = 0;
        int mHeight = 0;
        float mDpiX = 0.0f;
        float mDpiY = 0.0f;
        bool mHasBackend = false;
        Displayer* mOuter = nullptr;
        Win32Mutex mRenderMutex;
        Win32Mutex mLendMutex;
        bool mInRendering = false;
        bool mNeedRecreateBitmap = false;
        D3D_FEATURE_LEVEL mCurrentFeatureLevel;
        ComPtr<ID3D11Device> mD3DDevice;
        ComPtr<ID3D11DeviceContext> mD3DDeviceContext;
        ComPtr<IDXGIFactory2> mDxgiFactory;
        ComPtr<IDXGIDevice1> mDxgiDevice;
        ComPtr<IDXGISurface> mDxgiSurface;
        ComPtr<ID2D1Bitmap1> mSurfaceBitmap;
        ComPtr<IDXGISwapChain1> mSwapChain;
        ComPtr<IDWriteFactory> mDWriteFactory;
        ComPtr<ID2D1Factory1> mD2DFactory;
        ComPtr<ID2D1Device> mD2DDevice;
        ComPtr<ID2D1DeviceContext> mDeviceContext;
        ComPtr<ID2D1DeviceContext> mLendContext;
#if !defined(_WTF_BUILD_WIN7) && !defined(_WTF_BUILD_UWP)
        ComPtr<IDCompositionDevice> mDCompDevice;
        ComPtr<IDCompositionTarget> mDCompTarget;
        ComPtr<IDCompositionVisual> mDCompVisual;
#endif
    };

}

#endif // _WTF_DISPLAYER_IMPL_HPP