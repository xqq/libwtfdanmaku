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
            if (mHasMovement) {
                mDeltaX = (mDestPoint.x - mSrcPoint.x) / mMoveDuration;
                mDeltaY = (mDestPoint.y - mSrcPoint.y) / mMoveDuration;
            }
            mDpiScaled = true;
        }

        if (!mHasMovement) {    // no mDestPoint?
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
        } else if (timediff >= mOffsetTime + mMoveDuration) {
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

    D2D1_MATRIX_4X4_F PositionDanmaku::GetPerspectiveTransformAtTime(Displayer* displayer, time_t time) {
        if (!mHasTransform) {
            return mMatrix;
        }

        Rect<float> rect = GetRectAtTime(displayer, time);

        mMatrix = D2D1::Matrix4x4F::Translation(-rect.left, -rect.top, 0) *
                  D2D1::Matrix4x4F::RotationY(-(float)mRotateY) *
                  D2D1::Matrix4x4F::RotationZ((float)mRotateZ) *
                  D2D1::Matrix4x4F::Translation(rect.left, rect.top, 0) *
                  D2D1::Matrix4x4F::PerspectiveProjection(500.0f);

        return mMatrix;
    }

    void PositionDanmaku::Calculate() {
        float alphaDiff = mDestAlpha - mSrcAlpha;
        mAlphaDelta = alphaDiff / mDuration;

        if (mRotateZ != 0 || mRotateY != 0) {
            mHasTransform = true;
        } else {
            mHasTransform = false;
            mMatrix = D2D1::Matrix4x4F::PerspectiveProjection(0);
        }

        if (mHasMovement) {
            mMoveDuration = mDuration - mOffsetTime - mDelayAfterStop;
        }

    }

    std::wstring PositionDanmaku::GetCustomFontName() {
        return mCustomFontName;
    }

}