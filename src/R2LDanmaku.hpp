#ifndef _WTF_R2L_DANMAKU_HPP
#define _WTF_R2L_DANMAKU_HPP

#include "Noncopyable.hpp"
#include "BaseDanmaku.hpp"

namespace WTFDanmaku {

    class IDanmakusRetainer;

    class R2LDanmaku : public BaseDanmaku {
    public:
        static inline DanmakuRef Create() {
            return xl::RefPtr<R2LDanmaku>(new R2LDanmaku);
        }
        static std::unique_ptr<IDanmakusRetainer> CreateRetainer();
    public:
        explicit R2LDanmaku() = default;

        virtual ~R2LDanmaku() override;

        virtual DanmakuType GetType() override;

        virtual void Measure(Displayer* displayer, DanmakuConfig* config) override;

        virtual void Layout(Displayer* displayer, DanmakuConfig* config, float x, float y) override;

        virtual bool IsAlive(time_t time) override;

        virtual float GetSpeed() override;

        virtual float GetLeftAtTime(Displayer* displayer, time_t time) override;

        virtual Rect<float> GetRectAtTime(Displayer* displayer, time_t time) override;
    private:
        class R2LRetainer;
    private:
        float y = 0.0f;
        float mSpeed = 0.0f;
    };

}

#endif // _WTF_R2L_DANMAKU_HPP