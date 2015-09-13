#ifndef _WTF_TOP_DANMAKU_HPP
#define _WTF_TOP_DANMAKU_HPP

#include "Noncopyable.hpp"
#include "BaseDanmaku.hpp"

namespace WTFDanmaku {

    class IDanmakusRetainer;

    class TopDanmaku : public BaseDanmaku {
    public:
        static inline DanmakuRef Create() {
            return xl::RefPtr<TopDanmaku>(new TopDanmaku);
        }
        static std::unique_ptr<IDanmakusRetainer> CreateRetainer();
    public:
        explicit TopDanmaku() = default;

        virtual ~TopDanmaku() override;

        virtual DanmakuType GetType() override;

        virtual void Layout(Displayer* displayer, DanmakuConfig* config, float x, float y) override;

        virtual float GetSpeed() override;

        virtual float GetLeftAtTime(Displayer* displayer, time_t time) override;

        virtual Rect<float> GetRectAtTime(Displayer* displayer, time_t time) override;
    private:
        class TopRetainer;
    private:
        float y = 0.0f;
    };

}

#endif // _WTF_TOP_DANMAKU_HPP