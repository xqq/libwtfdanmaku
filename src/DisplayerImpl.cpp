#include <cmath>
#include "Renderable.hpp"
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

        hr = mD2DFactory->CreateDevice(mDxgiDevice.Get(), &mD2DDevice);
        if (FAILED(hr))
            return false;

        hr = mD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &mDeviceContext);
        if (FAILED(hr))
            return false;

        D2D1_BITMAP_PROPERTIES1 props = {};
        props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
        props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

        hr = mDeviceContext->CreateBitmapFromDxgiSurface(mDxgiSurface.Get(), props, &mSurfaceBitmap);
        if (FAILED(hr))
            return false;

        mDeviceContext->SetTarget(mSurfaceBitmap.Get());

        mD2DFactory->GetDesktopDpi(&mDpiX, &mDpiY);

        //D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
        //    D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), mDpiX, mDpiY);

        //hr = mD2DFactory->CreateDxgiSurfaceRenderTarget(mDxgiSurface.Get(), props, &mRenderTarget);
        //if (FAILED(hr))
        //    return false;

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
    
    void DisplayerImpl::Resize(int width, int height) {

    }

    ComPtr<ID2D1Bitmap1> DisplayerImpl::CreateBitmap(float width, float height) {
        if (!mHasBackend)
            return nullptr;

        /*ComPtr<ID3D10Texture2D> texture;

        D3D10_TEXTURE2D_DESC texdesc = { 0 };
        texdesc.ArraySize = 1;
        texdesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
        texdesc.CPUAccessFlags = 0;
        texdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        texdesc.Width = static_cast<UINT>(std::ceilf(width));
        texdesc.Height = static_cast<UINT>(std::ceilf(height));
        texdesc.MipLevels = 1;
        texdesc.MiscFlags = 0;
        texdesc.SampleDesc.Count = 1;
        texdesc.SampleDesc.Quality = 0;
        texdesc.Usage = D3D10_USAGE_DEFAULT;

        HRESULT hr = mD3DDevice->CreateTexture2D(&texdesc, nullptr, &texture);
        if (FAILED(hr))
            return nullptr;

        ComPtr<IDXGISurface> texSurface;
        hr = texture.As(&texSurface);
        if (FAILED(hr))
            return nullptr;

        D2D1_BITMAP_PROPERTIES1 props = {};
        props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
        props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;

        ComPtr<ID2D1Bitmap1> bitmap;

        hr = mDeviceContext->CreateBitmapFromDxgiSurface(texSurface.Get(), &props, &bitmap);
        if (FAILED(hr))
            return nullptr;*/

        D2D1_SIZE_U size;
        size.width = static_cast<uint32_t>(std::ceilf(width));
        size.height = static_cast<uint32_t>(std::ceilf(height));

        D2D1_BITMAP_PROPERTIES1 props = {};
        props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;
        props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;

        ComPtr<ID2D1Bitmap1> bitmap;

        HRESULT hr = mDeviceContext->CreateBitmap(size, nullptr, size.width * 4, props, &bitmap);
        if (FAILED(hr))
            return nullptr;

        return bitmap;
    }

    ComPtr<ID2D1RenderTarget> DisplayerImpl::AcquireRenderTarget(ComPtr<ID2D1Bitmap1> bitmap) {
        HRESULT hr = mDeviceContext->EndDraw();
        mDeviceContext->SetTarget(bitmap.Get());

        return mDeviceContext;
    }

    void DisplayerImpl::ReleaseRenderTarget(ComPtr<ID2D1RenderTarget> renderTarget) {
        mDeviceContext->SetTarget(mSurfaceBitmap.Get());
        mDeviceContext->BeginDraw();
    }

    ComPtr<IDWriteFactory> DisplayerImpl::GetDWriteFactory() {
        return mDWriteFactory;
    }

    void DisplayerImpl::DrawDanmakuItem(DanmakuRef item, time_t current) {
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
            renderable->BuildBitmap(mOuter);
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