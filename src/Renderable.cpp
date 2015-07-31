#include "GlobalConfig.hpp"
#include "BaseDanmaku.hpp"
#include "Displayer.hpp"
#include "Renderable.hpp"


namespace WTFDanmaku {

    bool Renderable::BuildTextLayout(ComPtr<IDWriteFactory> dwFactory) {
        ComPtr<IDWriteTextFormat> textFormat = nullptr;

        HRESULT hr = dwFactory->CreateTextFormat(sFontName, nullptr, sFontWeight, sFontStyle,
            sFontStretch, mDanmaku->mTextSize * sFontScaleFactor, L"zh-cn", &textFormat);
        if (FAILED(hr) || nullptr == textFormat)
            return false;

        textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

        hr = dwFactory->CreateTextLayout(mDanmaku->mComment.c_str(), mDanmaku->mComment.length(), textFormat.Get(), 4096.f, 1024.f, &mTextLayout);
        if (FAILED(hr) || nullptr == mTextLayout)
            return false;

        DWRITE_TEXT_METRICS metrics;
        mTextLayout->GetMetrics(&metrics);
        mDanmaku->mTextWidth = metrics.width;
        mDanmaku->mTextHeight = metrics.height;

        return true;
    }
        
    bool Renderable::BuildBitmap(Displayer* displayer) {
        if (!HasTextLayout())
            return false;

        DWRITE_TEXT_METRICS metrices;
        HRESULT hr = mTextLayout->GetMetrics(&metrices);
        if (FAILED(hr))
            return false;

        auto bmp = displayer->CreateBitmap(metrices.width, metrices.height);
        if (bmp == nullptr)
            return false;

        auto renderTarget = displayer->ObtainRenderTarget(bmp);
        if (renderTarget == nullptr)
            return false;

        renderTarget->BeginDraw();

        ComPtr<ID2D1SolidColorBrush> brush;
        renderTarget->CreateSolidColorBrush(D2D1::ColorF(mDanmaku->mTextColor), &brush);
        renderTarget->DrawTextLayout(D2D1::Point2F(), mTextLayout.Get(), brush.Get());

        hr = renderTarget->EndDraw();
        if (FAILED(hr))
            return false;

        mBitmap = bmp;
        return true;
    }

}