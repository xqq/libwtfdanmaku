#include "Displayer.hpp"
#include "R2LDanmaku.hpp"

namespace WTFDanmaku {

    R2LDanmaku::~R2LDanmaku() {
        
    }

    DanmakuType R2LDanmaku::GetType() {
        return kScrolling;
    }

    void R2LDanmaku::Measure(Displayer* displayer) {
        this->BaseDanmaku::Measure(displayer);
        mSpeed = (sLogicalScreenWidth + mTextWidth) / sDanmakuDuration;
    }

    void R2LDanmaku::Layout(Displayer* displayer, float x, float y) {
        this->y = y;
        mRect.top = y;
        int screenWidth = displayer->GetWidth();
        mDuration = static_cast<time_t>((screenWidth + mTextWidth) / mSpeed);
        mHasLayout = true;
    }

    bool R2LDanmaku::IsAlive(time_t time) {
        time_t elapsed = time - this->mStartTime;
        return elapsed > 0 && elapsed <= mDuration;
    }

    float R2LDanmaku::GetSpeed() {
        return mSpeed;
    }

    float R2LDanmaku::GetLeftAtTime(Displayer* displayer, time_t time) {
        time_t elapsedTime = time - this->mStartTime;
        
        return displayer->GetWidth() - elapsedTime * mSpeed;
    }

    Rect<float> R2LDanmaku::GetRectAtTime(Displayer* displayer, time_t time) {
        mRect.left = GetLeftAtTime(displayer, time);
        mRect.right = mRect.left + mTextWidth;
        mRect.top = y;
        mRect.bottom = mRect.top + mTextHeight;
        
        return mRect;
    }

}