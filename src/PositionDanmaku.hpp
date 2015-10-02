#ifndef _WTF_POSITION_DANMAKU_HPP
#define _WTF_POSITION_DANMAKU_HPP

#include <d2d1_1.h>
#include "BaseDanmaku.hpp"
#include "Point.hpp"

namespace WTFDanmaku {

    class PositionDanmaku : public BaseDanmaku {
    public:
        static inline DanmakuRef Create() {
            return xl::RefPtr<PositionDanmaku>(new PositionDanmaku());
        }
    public:
        PositionDanmaku() = default;
        virtual ~PositionDanmaku() = default;
        virtual DanmakuType GetType() override;
        virtual void Layout(Displayer* displayer, DanmakuConfig* config, float x, float y) override;
        virtual float GetSpeed() override;
        virtual float GetLeftAtTime(Displayer* displayer, time_t time) override;
        virtual Rect<float> GetRectAtTime(Displayer* displayer, time_t time) override;
        virtual D2D1_MATRIX_3X2_F GetTransform() override;
        float GetOpacityAtTime(time_t time);
    private:
        void Calculate();
    private:
        Point<float> mSrcPoint;
        Point<float> mDestPoint;
        float mDeltaX = 0.0f;
        float mDeltaY = 0.0f;
        float mSrcAlpha = 1.0f;
        float mDestAlpha = 1.0f;
        float mAlphaDelta = 0.0f;
        int mRotateZ = 0;
        int mRotateY = 0;
        time_t mOffsetTime = 0;
        time_t mDelayAfterStop = 0;
        bool mDpiScaled = false;
        bool mHasMovement = false;
        bool mHasCustomFont = false;
        std::wstring mCustomFontName;
        time_t mMoveDuration = 0;
        D2D1_MATRIX_3X2_F mMatrix;
    private:
        friend class BilibiliParser;
    };

}

#endif // _WTF_POSITION_DANMAKU_HPP