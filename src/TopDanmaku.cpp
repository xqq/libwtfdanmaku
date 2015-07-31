#include "Displayer.hpp"
#include "TopDanmaku.hpp"

namespace WTFDanmaku {

    TopDanmaku::~TopDanmaku() {

    }

    DanmakuType TopDanmaku::GetType() {
        return kTop;
    }

    void TopDanmaku::Layout(Displayer* displayer, float x, float y) {
        this->y = y;
        mRect.top = y;
        mHasLayout = true;
    }

    float TopDanmaku::GetSpeed() {
        return 0.0f;
    }

    float TopDanmaku::GetLeftAtTime(Displayer* displayer, time_t time) {
        float screenWidth = static_cast<float>(displayer->GetWidth());
        if (!this->HasMeasured())
            this->Measure(displayer);

        return screenWidth / 2 - mTextWidth / 2;
    }

    Rect<float> TopDanmaku::GetRectAtTime(Displayer* displayer, time_t time) {
        mRect.left = GetLeftAtTime(displayer, time);
        mRect.right = mRect.left + mTextWidth;
        mRect.top = y;
        mRect.bottom = mRect.top + mTextHeight;

        return mRect;
    }

}