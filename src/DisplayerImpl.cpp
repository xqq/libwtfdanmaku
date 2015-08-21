#include <cmath>
#include <mutex>
#include "Renderable.hpp"
#include "DanmakuConfig.hpp"
#include "DisplayerImpl.hpp"

namespace WTFDanmaku {

    DisplayerImpl::DisplayerImpl(Displayer* outer) : mOuter(outer) {

    }

    DisplayerImpl::~DisplayerImpl() {

    }

    void DisplayerImpl::SetTarget(HWND windowHandle) {
        mHwnd = windowHandle;
    }

    HRESULT DisplayerImpl::CreateD3D10Device(IDXGIAdapter* adapter, D3D10_DRIVER_TYPE driverType, UINT flags, ID3D10Device1** ppDevice) {
        HRESULT hr = S_OK;

        static const D3D10_FEATURE_LEVEL1 levelAttempts[] = {
            D3D10_FEATURE_LEVEL_10_1,
            D3D10_FEATURE_LEVEL_10_0,
            D3D10_FEATURE_LEVEL_9_3,
            D3D10_FEATURE_LEVEL_9_2,
            D3D10_FEATURE_LEVEL_9_1
        };

        for (int i = 0; i < sizeof(levelAttempts) / sizeof(levelAttempts[0]); i++) {
            ID3D10Device1* device = nullptr;
            hr = D3D10CreateDevice1(adapter, driverType, NULL, flags, levelAttempts[i], D3D10_1_SDK_VERSION, &device);
            if (SUCCEEDED(hr)) {
                *ppDevice = device;
                device = nullptr;
                break;
            }
        }

        return hr;
    }

    bool DisplayerImpl::SetupBackend() {
        if (mHasBackend)
            return false;

        if (mHwnd == NULL)
            return false;

        HRESULT hr = CreateD3D10Device(nullptr, D3D10_DRIVER_TYPE_HARDWARE, D3D10_CREATE_DEVICE_BGRA_SUPPORT, &mD3DDevice);
        if (FAILED(hr)) {    // fallback to software D3D10 backend
            hr = CreateD3D10Device(nullptr, D3D10_DRIVER_TYPE_WARP, D3D10_CREATE_DEVICE_BGRA_SUPPORT, &mD3DDevice);
        }

        if (FAILED(hr))
            return false;

        hr = mD3DDevice.As(&mDxgiDevice);
        if (FAILED(hr))
            return false;

        hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, __uuidof(mDxgiFactory), (void**)mDxgiFactory.GetAddressOf());
        if (FAILED(hr))
            return false;

        RECT rect = { 0 };
        GetClientRect(mHwnd, &rect);

        mWidth = rect.right - rect.left;
        mHeight = rect.bottom - rect.top;

        DXGI_SWAP_CHAIN_DESC1 description = { 0 };
        description.Width = mWidth;
        description.Height = mHeight;
        description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        description.BufferCount = 2;
        description.SampleDesc.Count = 1;
        description.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

        hr = mDxgiFactory->CreateSwapChainForComposition(mDxgiDevice.Get(), &description, nullptr, &mSwapChain);
        if (FAILED(hr))
            return false;

        hr = mSwapChain->GetBuffer(0, IID_PPV_ARGS(&mDxgiSurface));
        if (FAILED(hr))
            return false;

        hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&mDWriteFactory);
        if (FAILED(hr))
            return false;

        const D2D1_FACTORY_OPTIONS options = { D2D1_DEBUG_LEVEL_INFORMATION };
        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, options, mD2DFactory.GetAddressOf());
        if (FAILED(hr))
            return false;

        mD2DFactory->GetDesktopDpi(&mDpiX, &mDpiY);

        hr = mD2DFactory->CreateDevice(mDxgiDevice.Get(), &mD2DDevice);
        if (FAILED(hr))
            return false;

        hr = mD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &mDeviceContext);
        if (FAILED(hr))
            return false;

        hr = mD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &mLendContext);
        if (FAILED(hr))
            return false;

        D2D1_BITMAP_PROPERTIES1 props = {};
        props.dpiX = mDpiX;
        props.dpiY = mDpiY;
        props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
        props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

        hr = mDeviceContext->CreateBitmapFromDxgiSurface(mDxgiSurface.Get(), props, &mSurfaceBitmap);
        if (FAILED(hr))
            return false;

        mDeviceContext->SetTarget(mSurfaceBitmap.Get());

        hr = DCompositionCreateDevice(mDxgiDevice.Get(), IID_PPV_ARGS(&mDCompDevice));
        if (FAILED(hr))
            return false;

        hr = mDCompDevice->CreateTargetForHwnd(mHwnd, true, &mDCompTarget);
        if (FAILED(hr))
            return false;

        hr = mDCompDevice->CreateVisual(&mDCompVisual);
        if (FAILED(hr))
            return false;

        mDCompVisual->SetBitmapInterpolationMode(DCOMPOSITION_BITMAP_INTERPOLATION_MODE_LINEAR);
        mDCompVisual->SetCompositeMode(DCOMPOSITION_COMPOSITE_MODE_SOURCE_OVER);
        mDCompVisual->SetContent(mSwapChain.Get());

        mDCompTarget->SetRoot(mDCompVisual.Get());

        hr = mDCompDevice->Commit();
        if (FAILED(hr))
            return false;

        mHasBackend = true;
        return true;
    }

    bool DisplayerImpl::TeardownBackend() {
        if (mInRendering) {
            return false;
        }
        mDCompTarget.Reset();
        mDCompVisual.Reset();
        mDCompDevice.Reset();
        mLendContext.Reset();
        if (mDeviceContext != nullptr)
            mDeviceContext->SetTarget(nullptr);
        mDeviceContext.Reset();
        mSurfaceBitmap.Reset();
        mD2DDevice.Reset();
        mD2DFactory.Reset();
        mDWriteFactory.Reset();
        mDxgiSurface.Reset();
        mSwapChain.Reset();
        mDxgiDevice.Reset();
        mDxgiFactory.Reset();
        mD3DDevice.Reset();
        return true;
    }
    
    void DisplayerImpl::Resize(uint32_t width, uint32_t height) {
        if (static_cast<int>(width) <= 0 || static_cast<int>(height) <= 0)
            return;

        std::lock_guard<Win32Mutex> locker(mRenderMutex);

        mDeviceContext->SetTarget(nullptr);
        mSurfaceBitmap.Reset();
        mDxgiSurface.Reset();

        HRESULT hr = mSwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
        if (FAILED(hr))
            return;

        hr = mSwapChain->GetBuffer(0, IID_PPV_ARGS(&mDxgiSurface));
        if (FAILED(hr))
            return;

        D2D1_BITMAP_PROPERTIES1 props = {};
        props.dpiX = mDpiX;
        props.dpiY = mDpiY;
        props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
        props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

        hr = mDeviceContext->CreateBitmapFromDxgiSurface(mDxgiSurface.Get(), props, &mSurfaceBitmap);
        if (FAILED(hr))
            return;

        mWidth = width;
        mHeight = height;
        mDeviceContext->SetTarget(mSurfaceBitmap.Get());
    }

    ComPtr<ID2D1Bitmap1> DisplayerImpl::CreateBitmap(uint32_t width, uint32_t height) {
        if (!mHasBackend)
            return nullptr;

        D2D1_SIZE_U size;
        size.width = width;
        size.height = height;

        D2D1_BITMAP_PROPERTIES1 props = {};
        props.dpiX = mDpiX;
        props.dpiY = mDpiY;
        props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;
        props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;

        ComPtr<ID2D1Bitmap1> bitmap;

        std::lock_guard<Win32Mutex> locker(mLendMutex);
        HRESULT hr = mLendContext->CreateBitmap(size, nullptr, size.width * 4, props, &bitmap);
        if (FAILED(hr))
            return nullptr;

        return bitmap;
    }

    ComPtr<ID2D1RenderTarget> DisplayerImpl::AcquireRenderTarget(ComPtr<ID2D1Bitmap1> bitmap) {
        mLendMutex.lock();
        mLendContext->SetTarget(bitmap.Get());

        return mLendContext;
    }

    void DisplayerImpl::ReleaseRenderTarget(ComPtr<ID2D1RenderTarget> renderTarget) {
        mLendContext->SetTarget(nullptr);
        mLendMutex.unlock();
    }

    ComPtr<IDWriteFactory> DisplayerImpl::GetDWriteFactory() {
        return mDWriteFactory;
    }

    void DisplayerImpl::DrawDanmakuItem(DanmakuRef item, time_t current, DanmakuConfig* config) {
        if (!mHasBackend)
            return;

        if (!mInRendering)
            return;

        if (item.get() == nullptr)
            return;

        auto rect = item->GetRectAtTime(mOuter, current);
        weak_ptr<Renderable> r = item->GetRenderable();
        shared_ptr<Renderable> renderable = r.lock();

        if (renderable == nullptr)
            return;

        if (!renderable->HasBitmap()) {
            renderable->BuildBitmap(mOuter, config);
        }

        auto bitmap = renderable->GetBitmap();

        if (bitmap == nullptr)
            return;

        D2D1_SIZE_U size = bitmap->GetPixelSize();
        D2D1_RECT_F dest = D2D1::RectF(
            rect.left, rect.top,
            rect.left + size.width, rect.top + size.height
        );

        mDeviceContext->DrawBitmap(bitmap.Get(), dest);
    }

    void DisplayerImpl::BeginDraw() {
        mRenderMutex.lock();
        mInRendering = true;
        mDeviceContext->BeginDraw();
        mDeviceContext->Clear();
    }

    HRESULT DisplayerImpl::EndDraw() {
        HRESULT hr = mDeviceContext->EndDraw();
        mD3DDevice->Flush();
        mSwapChain->Present(1, 0);
        mInRendering = false;
        mRenderMutex.unlock();
        return hr;
    }

}