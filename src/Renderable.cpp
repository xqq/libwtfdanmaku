#include <cmath>
#include "BaseDanmaku.hpp"
#include "Displayer.hpp"
#include "Renderable.hpp"
#include "DanmakuConfig.hpp"
#include "OutlineTextRenderer.hpp"

namespace WTFDanmaku {

    bool Renderable::BuildTextLayout(Displayer* displayer, DanmakuConfig* config) {
        ComPtr<IDWriteFactory> dwFactory = displayer->GetDWriteFactory();
        if (nullptr == dwFactory)
            return false;

        ComPtr<IDWriteTextFormat> textFormat = nullptr;

        float fontSize = mDanmaku->mTextSize * config->FontScaleFactor;
        fontSize *= displayer->GetDpiY() / 96.0f;

        HRESULT hr = dwFactory->CreateTextFormat(config->FontName.c_str(), nullptr, config->FontWeight, config->FontStyle,
            config->FontStretch, fontSize, L"zh-cn", &textFormat);
        if (FAILED(hr) || nullptr == textFormat)
            return false;

        textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

        hr = dwFactory->CreateTextLayout(mDanmaku->mComment.c_str(), static_cast<UINT32>(mDanmaku->mComment.length()), textFormat.Get(), 8192.0f, 2160.0f, &mTextLayout);
        if (FAILED(hr) || nullptr == mTextLayout)
            return false;

        DWRITE_TEXT_METRICS metrics = {0};
        hr = mTextLayout->GetMetrics(&metrics);
        if (FAILED(hr))
            return false;

        mDanmaku->mTextWidth = std::ceilf(metrics.width);
        mDanmaku->mTextHeight = std::ceilf(metrics.height);

        return true;
    }

    bool Renderable::HasBitmap(DanmakuConfig* config) {
        return mBitmap.Get() != nullptr
            && mBitmapValidFlag == config->BitmapValidFlag;
    }

    bool Renderable::BuildBitmap(Displayer* displayer, DanmakuConfig* config) {
        if (!HasTextLayout())
            return false;

        ComPtr<ID2D1Factory1> d2dFactory = displayer->GetD2DFactory();
        if (nullptr == d2dFactory)
            return false;

        ComPtr<ID2D1Bitmap1> bmp = displayer->CreateBitmap(
            static_cast<uint32_t>(mDanmaku->mTextWidth),
            static_cast<uint32_t>(mDanmaku->mTextHeight)
        );
        if (bmp == nullptr)
            return false;

        float strokeWidth = 1.5f * config->FontScaleFactor;
        strokeWidth *= displayer->GetDpiY() / 96.0f;

        ComPtr<ID2D1RenderTarget> renderTarget = displayer->AcquireRenderTarget(bmp);
        if (renderTarget == nullptr)
            return false;

        renderTarget->BeginDraw();

        renderTarget->Clear();
        renderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

        ComPtr<ID2D1SolidColorBrush> brush;
        renderTarget->CreateSolidColorBrush(D2D1::ColorF(mDanmaku->mTextColor), &brush);

        ComPtr<ID2D1SolidColorBrush> outlineBrush;
        renderTarget->CreateSolidColorBrush(D2D1::ColorF(mDanmaku->mTextShadowColor, 1.0f), &outlineBrush);

        switch (config->DanmakuStyle) {
            case kOutline: {
                ComPtr<OutlineTextRenderer> textRenderer(new OutlineTextRenderer(d2dFactory, renderTarget, outlineBrush, strokeWidth, brush));
                mTextLayout->Draw(renderTarget.Get(), textRenderer.Get(), 0.0f, 0.0f);
                break;
            }
            case kProjection: {
                renderTarget->DrawTextLayout(D2D1::Point2F(1.0f, 1.0f), mTextLayout.Get(), outlineBrush.Get());
                renderTarget->DrawTextLayout(D2D1::Point2F(0.0f, 0.0f), mTextLayout.Get(), brush.Get());
                break;
            }
        }

        HRESULT hr = renderTarget->EndDraw();

        displayer->ReleaseRenderTarget(renderTarget);

        if (FAILED(hr))
            return false;

        mBitmap = bmp;
        mBitmapValidFlag = config->BitmapValidFlag;
        return true;
    }

}