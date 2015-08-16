#include "GlobalConfig.hpp"
#include "BaseDanmaku.hpp"
#include "Displayer.hpp"
#include "Renderable.hpp"

namespace WTFDanmaku {

    bool Renderable::BuildTextLayout(Displayer* displayer) {
        ComPtr<IDWriteFactory> dwFactory = displayer->GetDWriteFactory();
        if (nullptr == dwFactory)
            return false;

        ComPtr<IDWriteTextFormat> textFormat = nullptr;

        HRESULT hr = dwFactory->CreateTextFormat(sFontName, nullptr, sFontWeight, sFontStyle,
            sFontStretch, mDanmaku->mTextSize * sFontScaleFactor, L"zh-cn", &textFormat);
        if (FAILED(hr) || nullptr == textFormat)
            return false;

        textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

        hr = dwFactory->CreateTextLayout(mDanmaku->mComment.c_str(), mDanmaku->mComment.length(), textFormat.Get(), 8192.0f, 1024.0f, &mTextLayout);
        if (FAILED(hr) || nullptr == mTextLayout)
            return false;

        DWRITE_TEXT_METRICS metrics = {0};
        hr = mTextLayout->GetMetrics(&metrics);
        if (FAILED(hr))
            return false;

        mDanmaku->mTextWidth = metrics.width;
        mDanmaku->mTextHeight = metrics.height;

        return true;
    }
    
    bool Renderable::BuildBitmap(Displayer* displayer) {
        if (!HasTextLayout())
            return false;

        DWRITE_TEXT_METRICS metrices = {0};
        HRESULT hr = mTextLayout->GetMetrics(&metrices);
        if (FAILED(hr))
            return false;

        ComPtr<ID2D1Bitmap1> bmp = displayer->CreateBitmap(metrices.width, metrices.height);
        if (bmp == nullptr)
            return false;

        auto renderTarget = displayer->AcquireRenderTarget(bmp);
        if (renderTarget == nullptr)
            return false;

        renderTarget->BeginDraw();

        renderTarget->Clear();
        renderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

        ComPtr<ID2D1SolidColorBrush> bottomBrush;
        renderTarget->CreateSolidColorBrush(D2D1::ColorF(mDanmaku->mTextShadowColor, 0.9f), &bottomBrush);
        renderTarget->DrawTextLayout(D2D1::Point2F(1.0f, 1.0f), mTextLayout.Get(), bottomBrush.Get());

        ComPtr<ID2D1SolidColorBrush> brush;
        renderTarget->CreateSolidColorBrush(D2D1::ColorF(mDanmaku->mTextColor), &brush);
        renderTarget->DrawTextLayout(D2D1::Point2F(), mTextLayout.Get(), brush.Get());

        hr = renderTarget->EndDraw();

        displayer->ReleaseRenderTarget(renderTarget);

        if (FAILED(hr))
            return false;

        mBitmap = bmp;
        return true;
    }

}