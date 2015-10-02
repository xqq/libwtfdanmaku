#include <d2d1_1.h>
#include "Displayer.hpp"
#include "DanmakuConfig.hpp"
#include "PositionDanmaku.hpp"

namespace WTFDanmaku {

    DanmakuType PositionDanmaku::GetType() {
        return DanmakuType::kPosition;
    }

    void PositionDanmaku::Layout(Displayer* displayer, DanmakuConfig* config, float x, float y) {
        mLayoutFlag = config->LayoutFlag;
    }

    float PositionDanmaku::GetSpeed() {
        return 0.0f;
    }

    float PositionDanmaku::GetLeftAtTime(Displayer* displayer, time_t time) {
        return GetRectAtTime(displayer, time).left;
    }

    Rect<float> PositionDanmaku::GetRectAtTime(Displayer* displayer, time_t time) {
        if (!mDpiScaled) {
            float scaleX = displayer->GetDpiX() / 96.0f;
            float scaleY = displayer->GetDpiY() / 96.0f;
            mSrcPoint.x *= scaleX;
            mSrcPoint.y *= scaleY;
            mDestPoint.x *= scaleX;
            mDestPoint.y *= scaleY;
            mDeltaX = (mDestPoint.x - mSrcPoint.x) / mMoveDuration;
            mDeltaY = (mDestPoint.y - mSrcPoint.y) / mMoveDuration;
            mDpiScaled = true;
        }

        if (!mHasMovement) {
            if (mRect.left == 0 && mRect.right == 0) {
                mRect.left = mSrcPoint.x;
                mRect.right = mRect.left + mTextWidth;
                mRect.top = mSrcPoint.y;
                mRect.bottom = mRect.top + mTextHeight;
            }
            return mRect;
        }

        int64_t timediff = time - mStartTime;

        if (timediff <= mOffsetTime) {
            timediff = 0;
        } else if (timediff >= mDuration - mDelayAfterStop) {
            mRect.left = mDestPoint.x;
            mRect.right = mRect.left + mTextWidth;
            mRect.top = mDestPoint.y;
            mRect.bottom = mRect.top + mTextHeight;
            return mRect;
        } else {
            timediff -= mOffsetTime;
        }

        mRect.left = mSrcPoint.x + mDeltaX * timediff;
        mRect.right = mRect.left + mTextWidth;
        mRect.top = mSrcPoint.y + mDeltaY * timediff;
        mRect.bottom = mRect.top + mTextHeight;
        return mRect;
    }

    float PositionDanmaku::GetOpacityAtTime(time_t time) {
        int64_t diff = time - mStartTime;
        if (diff <= 0)
            return mSrcAlpha;

        return mSrcAlpha + mAlphaDelta * diff;
    }

    D2D1_MATRIX_3X2_F PositionDanmaku::GetTransform() {
        return mMatrix;
    }

    void PositionDanmaku::Calculate() {
        float alphaDiff = mDestAlpha - mSrcAlpha;
        mAlphaDelta = alphaDiff / mDuration;

        if (mRotateZ != 0 || mRotateY != 0) {
            mHasTransform = true;
            mMatrix = D2D1::Matrix3x2F::Identity();
        }

        if (mHasMovement) {
            mMoveDuration = mDuration - mOffsetTime - mDelayAfterStop;
        }

    }
}