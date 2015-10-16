#include <cmath>
#include <mutex>
#include "Renderable.hpp"
#include "DanmakuConfig.hpp"
#include "DisplayerImpl.hpp"

#if !defined(_WTF_BUILD_WIN7) && !defined(_WTF_BUILD_UWP)
#pragma comment (lib, "dcomp.lib")
#endif

namespace WTFDanmaku {

    DisplayerImpl::DisplayerImpl(Displayer* outer) : mOuter(outer) {

    }

    DisplayerImpl::~DisplayerImpl() {
        if (mHasBackend) {
            TeardownBackend();
        }
    }

    void DisplayerImpl::SetTarget(HWND windowHandle, uint32_t initialWidth, uint32_t initialHeight) {
        mHwnd = windowHandle;
        mWidth = initialWidth;
        mHeight = initialHeight;
    }

    bool DisplayerImpl::SetupBackend() {
        if (mHasBackend)
            return false;

#ifdef _WTF_BUILD_WIN7
        if (mHwnd == NULL)
            return false;
#endif

        if (mHwnd == NULL && (mWidth == 0 || mHeight == 0))
            return false;

        HRESULT hr = CreateDeviceIndependentResources();
        if (FAILED(hr))
            return false;

        hr = CreateDeviceResources();
        if (FAILED(hr))
            return false;

        hr = CreateTargetDependentResources();
        if (FAILED(hr))
            return false;

        if (mHwnd) {
            hr = CreateDCompResources();
            if (FAILED(hr))
                return false;
        }

        mHasBackend = true;
        return true;
    }

    HRESULT DisplayerImpl::QuerySwapChain(const IID* pGuid, void** ppvObject) {
        if (pGuid == nullptr || ppvObject == nullptr || mSwapChain == nullptr)
            return E_FAIL;

        if (__uuidof(IDXGISwapChain) == *pGuid) {
            *ppvObject = static_cast<IDXGISwapChain*>(mSwapChain.Get());
        } else if (__uuidof(IDXGISwapChain1) == *pGuid) {
            *ppvObject = mSwapChain.Get();
        } else {
            *ppvObject = nullptr;
            return E_FAIL;
        }

        mSwapChain.Get()->AddRef();
        return S_OK;
    }

    HRESULT DisplayerImpl::CreateDeviceIndependentResources() {
        HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&mDWriteFactory);
        if (FAILED(hr))
            return hr;

        D2D1_FACTORY_OPTIONS options;
        memset(&options, 0, sizeof(options));

#ifdef _DEBUG
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, options, mD2DFactory.GetAddressOf());
        if (FAILED(hr))
            return hr;

        mD2DFactory->GetDesktopDpi(&mDpiX, &mDpiY);
        return hr;
    }

    HRESULT DisplayerImpl::CreateD3D11Device(IDXGIAdapter* adapter, D3D_DRIVER_TYPE driverType, UINT flags,
                                             ID3D11Device** ppDevice, ID3D11DeviceContext** ppDevCtx, D3D_FEATURE_LEVEL* resultLevel) {
        HRESULT hr = S_OK;

        static const D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
        };

        uint32_t arraySize = sizeof(featureLevels) / sizeof(featureLevels[0]);

        D3D_FEATURE_LEVEL result;
        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* devctx = nullptr;

        hr = D3D11CreateDevice(adapter, driverType, 0, flags, featureLevels, arraySize, D3D11_SDK_VERSION, &device, &result, &devctx);
        if (FAILED(hr))
            return hr;

        if (resultLevel) {
            *resultLevel = result;
        }
        *ppDevice = device;
        *ppDevCtx = devctx;

        return hr;
    }

    HRESULT DisplayerImpl::CreateDeviceResources() {
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

        HRESULT hr = CreateD3D11Device(nullptr, D3D_DRIVER_TYPE_HARDWARE, creationFlags, &mD3DDevice, &mD3DDeviceContext, &mCurrentFeatureLevel);
        if (FAILED(hr)) {    // fallback to software WARP device
            hr = CreateD3D11Device(nullptr, D3D_DRIVER_TYPE_WARP, creationFlags, &mD3DDevice, &mD3DDeviceContext, &mCurrentFeatureLevel);
        }

        if (FAILED(hr))
            return hr;

        hr = mD3DDevice.As(&mDxgiDevice);
        if (FAILED(hr))
            return hr;

        hr = mD2DFactory->CreateDevice(mDxgiDevice.Get(), &mD2DDevice);
        if (FAILED(hr))
            return hr;

        hr = mD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &mDeviceContext);
        if (FAILED(hr))
            return hr;

        hr = mD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &mLendContext);
        if (FAILED(hr))
            return hr;

        return hr;
    }

    HRESULT DisplayerImpl::CreateTargetDependentResources() {
#ifndef _WTF_BUILD_UWP
        if (mHwnd) {
            RECT rect = { 0 };
            GetClientRect(mHwnd, &rect);

            mWidth = rect.right - rect.left;
            mHeight = rect.bottom - rect.top;
        }
#endif

        DXGI_SWAP_CHAIN_DESC1 description = { 0 };

        description.Width = mWidth;
        description.Height = mHeight;
        description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        description.Stereo = FALSE;
        description.SampleDesc.Count = 1;
        description.SampleDesc.Quality = 0;
        description.BufferCount = 2;
        description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        description.Scaling = DXGI_SCALING_STRETCH;
#ifdef _WTF_BUILD_WIN7
        description.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        description.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
#else
        description.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
        description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
#endif

        HRESULT hr = mD3DDevice.As(&mDxgiDevice);
        if (FAILED(hr))
            return hr;

        ComPtr<IDXGIAdapter> dxgiAdapter;
        hr = mDxgiDevice->GetAdapter(&dxgiAdapter);
        if (FAILED(hr))
            return hr;

        hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&mDxgiFactory));
        if (FAILED(hr))
            return hr;

#ifdef _WTF_BUILD_WIN7
        hr = mDxgiFactory->CreateSwapChainForHwnd(mD3DDevice.Get(), mHwnd, &description, nullptr, nullptr, &mSwapChain);
#else
        hr = mDxgiFactory->CreateSwapChainForComposition(mDxgiDevice.Get(), &description, nullptr, &mSwapChain);
#endif
        if (FAILED(hr))
            return hr;

        hr = mDxgiDevice->SetMaximumFrameLatency(1);
        if (FAILED(hr))
            return hr;

        hr = mSwapChain->GetBuffer(0, IID_PPV_ARGS(&mDxgiSurface));
        if (FAILED(hr))
            return hr;

        D2D1_BITMAP_PROPERTIES1 props = {};
        props.dpiX = mDpiX;
        props.dpiY = mDpiY;
        props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
        props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

        hr = mDeviceContext->CreateBitmapFromDxgiSurface(mDxgiSurface.Get(), props, &mSurfaceBitmap);
        if (FAILED(hr))
            return hr;

        mDeviceContext->SetTarget(mSurfaceBitmap.Get());

        return hr;
    }

    HRESULT DisplayerImpl::CreateDCompResources() {
        HRESULT hr = S_OK;
#if !defined(_WTF_BUILD_WIN7) && !defined(_WTF_BUILD_UWP)
        hr = DCompositionCreateDevice(mDxgiDevice.Get(), IID_PPV_ARGS(&mDCompDevice));
        if (FAILED(hr))
            return hr;

        hr = mDCompDevice->CreateTargetForHwnd(mHwnd, true, &mDCompTarget);
        if (FAILED(hr))
            return hr;

        hr = mDCompDevice->CreateVisual(&mDCompVisual);
        if (FAILED(hr))
            return hr;

        mDCompVisual->SetBitmapInterpolationMode(DCOMPOSITION_BITMAP_INTERPOLATION_MODE_LINEAR);
        mDCompVisual->SetCompositeMode(DCOMPOSITION_COMPOSITE_MODE_SOURCE_OVER);
        mDCompVisual->SetContent(mSwapChain.Get());

        mDCompTarget->SetRoot(mDCompVisual.Get());

        hr = mDCompDevice->Commit();
        if (FAILED(hr))
            return hr;
#endif
        return hr;
    }

    HRESULT DisplayerImpl::HandleDeviceLost() {
        HRESULT hr = CreateDeviceResources();
        if (FAILED(hr))
            return hr;

        hr = CreateTargetDependentResources();
        if (FAILED(hr))
            return hr;

        if (mHwnd) {
            hr = CreateDCompResources();
            if (FAILED(hr))
                return hr;
        }

        mNeedRecreateBitmap = true;

        return hr;
    }

    bool DisplayerImpl::TeardownBackend() {
        if (mInRendering) {
            return false;
        }
#if !defined(_WTF_BUILD_WIN7) && !defined(_WTF_BUILD_UWP)
        mDCompTarget.Reset();
        mDCompVisual.Reset();
        mDCompDevice.Reset();
#endif
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
        mD3DDeviceContext.Reset();
        mD3DDevice.Reset();

        mHasBackend = false;
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

    void DisplayerImpl::SetDpi(uint32_t dpiX, uint32_t dpiY) {
        mDpiX = static_cast<float>(dpiX);
        mDpiY = static_cast<float>(dpiY);
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

    ComPtr<ID2D1DeviceContext> DisplayerImpl::AcquireDeviceContext(ComPtr<ID2D1Bitmap1> bitmap) {
        mLendMutex.lock();
        mLendContext->SetTarget(bitmap.Get());

        return mLendContext;
    }

    void DisplayerImpl::ReleaseDeviceContext(ComPtr<ID2D1DeviceContext> deviceContext) {
        mLendContext->SetTarget(nullptr);
        mLendMutex.unlock();
    }

    ComPtr<ID2D1Factory1> DisplayerImpl::GetD2DFactory() {
        return mD2DFactory;
    }

    ComPtr<IDWriteFactory> DisplayerImpl::GetDWriteFactory() {
        return mDWriteFactory;
    }

    void DisplayerImpl::DrawDanmakuItem(DanmakuRef item, time_t current, DanmakuConfig* config) {
        if (!mHasBackend)
            return;

        if (!mInRendering)
            return;

        if (mNeedRecreateBitmap) {
            config->BitmapValidFlag++;
            mNeedRecreateBitmap = false;
        }

        if (item.Get() == nullptr)
            return;

        auto rect = item->GetRectAtTime(mOuter, current);
        weak_ptr<Renderable> r = item->GetRenderable();
        shared_ptr<Renderable> renderable = r.lock();

        if (renderable == nullptr)
            return;

        if (!renderable->HasBitmap(config)) {
            renderable->BuildBitmap(mOuter, config);
        }

        auto bitmap = renderable->GetBitmap();

        if (bitmap == nullptr)
            return;

        float left = std::roundf(rect.left);
        float top = std::roundf(rect.top);

        D2D1_SIZE_U size = bitmap->GetPixelSize();
        D2D1_RECT_F dest = D2D1::RectF(
            left, top,
            left + size.width, top + size.height
        );

        mDeviceContext->DrawBitmap(bitmap.Get(), dest, config->CompositionOpacity, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
    }

    void DisplayerImpl::BeginDraw() {
        mRenderMutex.lock();
        mInRendering = true;
        mDeviceContext->BeginDraw();
        mDeviceContext->Clear();
    }

    HRESULT DisplayerImpl::EndDraw() {
        HRESULT hr = mDeviceContext->EndDraw();

        mInRendering = false;
        mRenderMutex.unlock();
        return hr;
    }

    HRESULT DisplayerImpl::Present() {
        HRESULT hr = mSwapChain->Present(1, 0);
        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
            hr = HandleDeviceLost();
        return hr;
    }
}