#ifndef _WTF_TOP_DANMAKU_HPP
#define _WTF_TOP_DANMAKU_HPP

#include "Noncopyable.hpp"
#include "BaseDanmaku.hpp"

namespace WTFDanmaku {

    class TopDanmaku : public BaseDanmaku {
    public:
        static inline DanmakuRef Create() {
            return std::make_shared<TopDanmaku>();
        }
    public:
        explicit TopDanmaku() = default;

        virtual ~TopDanmaku() override;

        virtual DanmakuType GetType() override;

        virtual void Layout(Displayer* displayer, float x, float y) override;

        virtual float GetSpeed() override;

        virtual float GetLeftAtTime(Displayer* displayer, time_t time) override;

        virtual Rect<float> GetRectAtTime(Displayer* displayer, time_t time) override;
    private:
        float y = 0.0f;
    };

}

#endif // _WTF_TOP_DANMAKU_HPP