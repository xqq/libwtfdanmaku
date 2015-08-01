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

    bool DisplayerImpl::SetupBackend() {
        if (mHasBackend)
            return false;

        if (mHwnd == NULL)
            return false;

        HRESULT hr = D3D10CreateDevice1(nullptr, D3D10_DRIVER_TYPE_HARDWARE, nullptr, D3D10_CREATE_DEVICE_BGRA_SUPPORT,
            D3D10_FEATURE_LEVEL_10_1, D3D10_SDK_VERSION, &mD3DDevice);
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

        float dpiX, dpiY;
        mD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), dpiX, dpiY);

        hr = mD2DFactory->CreateDxgiSurfaceRenderTarget(mDxgiSurface.Get(), props, &mRenderTarget);
        if (FAILED(hr))
            return false;

        mHasBackend = true;
        
        return true;
    }

    bool DisplayerImpl::TeardownBackend() {
        return true;
    }

    int DisplayerImpl::GetWidth() {
        return mWidth;
    }

    int DisplayerImpl::GetHeight() {
        return mHeight;
    }

    float DisplayerImpl::GetDpiX() {
        return 0;
    }

    float DisplayerImpl::GetDpiY() {
        return 0;
    }
    
    void DisplayerImpl::Resize(int width, int height) {

    }

    ComPtr<ID2D1Bitmap1> DisplayerImpl::CreateBitmap(float width, float height) {
        if (!mHasBackend)
            return nullptr;

        ComPtr<ID2D1Bitmap1> bitmap;
        ComPtr<ID3D10Texture2D> texture;
        ComPtr<IDXGISurface> texSurface;

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

        hr = texture.As(&texSurface);
        if (FAILED(hr))
            return nullptr;

        D2D1_BITMAP_PROPERTIES1 props = {};
        props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
        props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

        hr = mDeviceContext->CreateBitmapFromDxgiSurface(texSurface.Get(), &props, &bitmap);
        if (FAILED(hr))
            return nullptr;

        return bitmap;
    }

    ComPtr<ID2D1RenderTarget> DisplayerImpl::ObtainRenderTarget(ComPtr<ID2D1Bitmap1> bitmap) {
        // TODO
        return nullptr;
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

        D2D1_RECT_F dest = D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom);
        
        mRenderTarget->DrawBitmap(bitmap.Get(), dest);
    }

    void DisplayerImpl::BeginDraw() {
        mRenderMutex.lock();
        mInRendering = true;
        mRenderTarget->BeginDraw();
    }

    HRESULT DisplayerImpl::EndDraw() {
        HRESULT hr = mRenderTarget->EndDraw();
        mInRendering = false;
        mRenderMutex.unlock();
        return hr;
    }

}