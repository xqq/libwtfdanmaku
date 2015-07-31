#ifndef _WTF_R2L_DANMAKU_HPP
#define _WTF_R2L_DANMAKU_HPP

#include "Noncopyable.hpp"
#include "BaseDanmaku.hpp"

namespace WTFDanmaku {

    class Displayer;

    class R2LDanmaku : public BaseDanmaku {
    public:
        explicit R2LDanmaku() = default;

        virtual ~R2LDanmaku() override;

        virtual DanmakuType GetType() override;

        virtual void Measure(Displayer* displayer) override;

        virtual void Layout(Displayer* displayer, float x, float y) override;

        virtual bool IsAlive(time_t time) override;

        virtual float GetSpeed() override;

        virtual float GetLeftAtTime(Displayer* displayer, time_t time) override;

        virtual Rect<float> GetRectAtTime(Displayer* displayer, time_t time) override;
    private:
        float y = 0.0f;
        float mSpeed = 0.0f;
        time_t mDuration = 0;
    };

}

#endif // _WTF_R2L_DANMAKU_HPP