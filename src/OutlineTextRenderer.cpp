#include "OutlineTextRenderer.hpp"

namespace WTFDanmaku {

    OutlineTextRenderer::OutlineTextRenderer(
        ComPtr<ID2D1Factory1> d2dFactory,
        ComPtr<ID2D1RenderTarget> renderTarget,
        ComPtr<ID2D1SolidColorBrush> outlineBrush,
        float outlineStrokeWidth,
        ComPtr<ID2D1SolidColorBrush> fillBrush
        ) :
        mRefCount(0),
        mD2DFactory(d2dFactory),
        mRenderTarget(renderTarget),
        mOutlineBrush(outlineBrush),
        mOutlineStrokeWidth(outlineStrokeWidth),
        mFillBrush(fillBrush)
    {

    }

    IFACEMETHODIMP OutlineTextRenderer::DrawGlyphRun(
        __maybenull void* clientDrawingContext,
        float baselineOriginX,
        float baselineOriginY,
        DWRITE_MEASURING_MODE measuringMode,
        __in DWRITE_GLYPH_RUN const* glyphRun,
        __in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
        IUnknown* clientDrawingEffect
        )
    {
        HRESULT hr = S_OK;

        ComPtr<ID2D1PathGeometry> pathGeometry;
        hr = mD2DFactory->CreatePathGeometry(&pathGeometry);

        ComPtr<ID2D1GeometrySink> sink;
        if (SUCCEEDED(hr)) {
            hr = pathGeometry->Open(&sink);
        }

        if (SUCCEEDED(hr)) {
            hr = glyphRun->fontFace->GetGlyphRunOutline(
                glyphRun->fontEmSize,
                glyphRun->glyphIndices,
                glyphRun->glyphAdvances,
                glyphRun->glyphOffsets,
                glyphRun->glyphCount,
                glyphRun->isSideways,
                glyphRun->bidiLevel % 2,
                sink.Get()
            );
        }

        if (SUCCEEDED(hr)) {
            hr = sink->Close();
        }

        const D2D1::Matrix3x2F matrix = D2D1::Matrix3x2F(
            1.0f, 0.0f,
            0.0f, 1.0f,
            baselineOriginX, baselineOriginY
        );

        ComPtr<ID2D1TransformedGeometry> transformedGeometry;
        if (SUCCEEDED(hr)) {
            hr = mD2DFactory->CreateTransformedGeometry(pathGeometry.Get(), matrix, &transformedGeometry);
        }

        mRenderTarget->DrawGeometry(transformedGeometry.Get(), mOutlineBrush.Get(), mOutlineStrokeWidth);
        mRenderTarget->FillGeometry(transformedGeometry.Get(), mFillBrush.Get());

        return hr;
    }

    IFACEMETHODIMP OutlineTextRenderer::DrawUnderline(
        __maybenull void* clientDrawingContext,
        float baselineOriginX,
        float baselineOriginY,
        __in DWRITE_UNDERLINE const* underline,
        IUnknown* clientDrawingEffect
        )
    {
        HRESULT hr;

        D2D1_RECT_F rect = D2D1::RectF(
            0,
            underline->offset,
            underline->width,
            underline->offset + underline->thickness
        );

        ComPtr<ID2D1RectangleGeometry> rectangleGeometry;
        hr = mD2DFactory->CreateRectangleGeometry(rect, &rectangleGeometry);

        const D2D1::Matrix3x2F matrix = D2D1::Matrix3x2F(
            1.0f, 0.0f,
            0.0f, 1.0f,
            baselineOriginX, baselineOriginY
        );

        ComPtr<ID2D1TransformedGeometry> transformedGeometry;
        if (SUCCEEDED(hr)) {
            hr = mD2DFactory->CreateTransformedGeometry(rectangleGeometry.Get(), matrix, &transformedGeometry);
        }

        mRenderTarget->DrawGeometry(transformedGeometry.Get(), mOutlineBrush.Get(), mOutlineStrokeWidth);
        mRenderTarget->FillGeometry(transformedGeometry.Get(), mFillBrush.Get());

        return S_OK;
    }

    IFACEMETHODIMP OutlineTextRenderer::DrawStrikethrough(
        __maybenull void* clientDrawingContext,
        float baselineOriginX,
        float baselineOriginY,
        __in DWRITE_STRIKETHROUGH const* strikeThrough,
        IUnknown* clientDrawingEffect
        )
    {
        HRESULT hr;

        D2D1_RECT_F rect = D2D1::RectF(
            0,
            strikeThrough->offset,
            strikeThrough->width,
            strikeThrough->offset + strikeThrough->thickness
        );

        ComPtr<ID2D1RectangleGeometry> rectangleGeometry;
        hr = mD2DFactory->CreateRectangleGeometry(rect, &rectangleGeometry);

        const D2D1::Matrix3x2F matrix = D2D1::Matrix3x2F(
            1.0f, 0.0f,
            0.0f, 1.0f,
            baselineOriginX, baselineOriginY
        );

        ComPtr<ID2D1TransformedGeometry> transformedGeometry;
        if (SUCCEEDED(hr)) {
            hr = mD2DFactory->CreateTransformedGeometry(rectangleGeometry.Get(), matrix, &transformedGeometry);
        }

        mRenderTarget->DrawGeometry(transformedGeometry.Get(), mOutlineBrush.Get(), mOutlineStrokeWidth);
        mRenderTarget->FillGeometry(transformedGeometry.Get(), mFillBrush.Get());

        return S_OK;
    }

    IFACEMETHODIMP OutlineTextRenderer::DrawInlineObject(
        __maybenull void* clientDrawingContext,
        float originX,
        float originY,
        IDWriteInlineObject* inlineObject,
        BOOL isSideways,
        BOOL isRightToLeft,
        IUnknown* clientDrawingEffect
        )
    {
        return E_NOTIMPL;
    }

    IFACEMETHODIMP OutlineTextRenderer::IsPixelSnappingDisabled(
        __maybenull void* clientDrawingContext,
        __out BOOL* isDisabled
        )
    {
        *isDisabled = FALSE;
        return S_OK;
    }

    IFACEMETHODIMP OutlineTextRenderer::GetCurrentTransform(
        __maybenull void* clientDrawingContext,
        __out DWRITE_MATRIX* transform
        )
    {
        mRenderTarget->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
        return S_OK;
    }

    IFACEMETHODIMP OutlineTextRenderer::GetPixelsPerDip(
        __maybenull void* clientDrawingContext,
        __out FLOAT* pixelsPreDip
        )
    {
        float x, y;
        mRenderTarget->GetDpi(&x, &y);
        *pixelsPreDip = x / 96;

        return S_OK;
    }

    IFACEMETHODIMP_(unsigned long) OutlineTextRenderer::AddRef() {
        return InterlockedIncrement(&mRefCount);
    }

    IFACEMETHODIMP_(unsigned long) OutlineTextRenderer::Release() {
        unsigned long result = InterlockedDecrement(&mRefCount);
        if (result == 0) {
            delete this;
            return 0;
        }
        return result;
    }

    IFACEMETHODIMP OutlineTextRenderer::QueryInterface(IID const& riid, void** ppvObject) {
        if (__uuidof(IDWriteTextRenderer) == riid) {
            *ppvObject = this;
        } else if (__uuidof(IDWritePixelSnapping) == riid) {
            *ppvObject = this;
        } else if (__uuidof(IUnknown) == riid) {
            *ppvObject = this;
        } else {
            *ppvObject = nullptr;
            return E_FAIL;
        }

        this->AddRef();
        return S_OK;
    }

}