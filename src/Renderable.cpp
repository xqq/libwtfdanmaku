#include "GlobalConfig.hpp"
#include "BaseDanmaku.hpp"
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
        
    bool Renderable::BuildBitmap() {
        return true;
    }

}