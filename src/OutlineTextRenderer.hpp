#ifndef _WTF_OUTLINE_TEXT_RENDERER_HPP
#define _WTF_OUTLINE_TEXT_RENDERER_HPP

#include <wrl.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <cstdint>

using Microsoft::WRL::ComPtr;

namespace WTFDanmaku {

    class OutlineTextRenderer : public IDWriteTextRenderer {
    public:
        explicit OutlineTextRenderer(
            ComPtr<ID2D1Factory1> d2dFactory,
            ComPtr<ID2D1RenderTarget> renderTarget,
            ComPtr<ID2D1SolidColorBrush> outlineBrush,
            float outlineStrokeWidth,
            ComPtr<ID2D1SolidColorBrush> fillBrush
        );

        ~OutlineTextRenderer() = default;

        IFACEMETHOD(IsPixelSnappingDisabled)(
            __maybenull void* clientDrawingContext,
            __out BOOL* isDisabled
        );

        IFACEMETHOD(GetCurrentTransform)(
            __maybenull void* clientDrawingContext,
            __out DWRITE_MATRIX* transform
        );

        IFACEMETHOD(GetPixelsPerDip)(
            __maybenull void* clientDrawingContext,
            __out FLOAT* pixelsPreDip
        );

        IFACEMETHOD(DrawGlyphRun)(
            __maybenull void* clientDrawingContext,
            float baselineOriginX,
            float baselineOriginY,
            DWRITE_MEASURING_MODE measuringMode,
            __in DWRITE_GLYPH_RUN const* glyphRun,
            __in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
            IUnknown* clientDrawingEffect
        );

        IFACEMETHOD(DrawUnderline)(
            __maybenull void* clientDrawingContext,
            float baselineOriginX,
            float baselineOriginY,
            __in DWRITE_UNDERLINE const* underline,
            IUnknown* clientDrawingEffect
        );

        IFACEMETHOD(DrawStrikethrough)(
            __maybenull void* clientDrawingContext,
            float baselineOriginX,
            float baselineOriginY,
            __in DWRITE_STRIKETHROUGH const* strikeThrough,
            IUnknown* clientDrawingEffect
        );

        IFACEMETHOD(DrawInlineObject)(
            __maybenull void* clientDrawingContext,
            float originX,
            float originY,
            IDWriteInlineObject* inlineObject,
            BOOL isSideways,
            BOOL isRightToLeft,
            IUnknown* clientDrawingEffect
        );
    public:
        IFACEMETHOD_(unsigned long, AddRef) ();
        IFACEMETHOD_(unsigned long, Release) ();
        IFACEMETHOD(QueryInterface)(
            IID const& riid,
            void** ppvObject
        );
    private:
        unsigned long mRefCount;
        ComPtr<ID2D1Factory1> mD2DFactory;
        ComPtr<ID2D1RenderTarget> mRenderTarget;
        ComPtr<ID2D1SolidColorBrush> mOutlineBrush;
        float mOutlineStrokeWidth;
        ComPtr<ID2D1SolidColorBrush> mFillBrush;
    };

}

#endif // _WTF_OUTLINE_TEXT_RENDERER_HPP