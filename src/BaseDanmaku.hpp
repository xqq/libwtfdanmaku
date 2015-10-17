#ifndef _WTF_BASE_DANMAKU_HPP
#define _WTF_BASE_DANMAKU_HPP

#include <cstdint>
#include <string>
#include <memory>
#include "base/RefBase.hpp"
#include "base/RefPtr.hpp"
#include "Noncopyable.hpp"
#include "Rect.hpp"

using std::shared_ptr;
using std::weak_ptr;

namespace WTFDanmaku {

    enum DanmakuType : int {
        kNull = 0,
        kScrolling = 1,
        kBottom = 4,
        kTop = 5,
        kReverse = 6,
        kPosition = 7,
        kAdvanced = 8
    };

    class Displayer;
    class Renderable;
    struct DanmakuConfig;
    
    class BaseDanmaku : public xl::RefBase {
        friend class DanmakuFactory;
        friend class Renderable;
    public:
        explicit BaseDanmaku();

        virtual ~BaseDanmaku();

        inline float GetWidth() {
            return mTextWidth;
        }

        inline float GetHeight() {
            return mTextHeight;
        }

        bool HasMeasured(DanmakuConfig* config);

        bool HasLayout(DanmakuConfig* config);

        inline bool HasRenderable() {
            return mRenderable != nullptr;
        }

        weak_ptr<Renderable> BuildRenderable(Displayer* displayer, DanmakuConfig* config);

        inline weak_ptr<Renderable> GetRenderable() {
            return weak_ptr<Renderable>(mRenderable);
        }

        inline void ReleaseRenderable() {
            mRenderable.reset();
        }

        virtual void ReleaseResources();

        virtual void Measure(Displayer* displayer, DanmakuConfig* config);

        virtual void Layout(Displayer* displayer, DanmakuConfig* config, float x, float y) = 0;

        virtual DanmakuType GetType() = 0;

        virtual bool IsAlive(time_t time);

        virtual float GetSpeed() = 0;

        virtual float GetLeftAtTime(Displayer* displayer, time_t time) = 0;

        virtual Rect<float> GetRectAtTime(Displayer* displayer, time_t time) = 0;

        inline time_t GetStartTime() {
            return mStartTime;
        }

        inline void SetStartTime(time_t time) {
            mStartTime = time;
        }

        inline Rect<float> GetRect() {
            return mRect;
        }

        inline time_t GetSendTimestamp() {
            return mTimestamp;
        }

        inline uint32_t GetDanmakuId() {
            return mDanmakuId;
        }

        inline bool IsSkipped() {
            return mSkipped;
        }

        inline void SetSkipped(bool skipped) {
            mSkipped = skipped;
        }
    protected:
        time_t mStartTime = 0;
        time_t mDuration = 0;
        Rect<float> mRect;
        float mTextWidth = -1.0f;
        float mTextHeight = -1.0f;
        int mLayoutFlag = -1;
    private:
        std::wstring mComment;
        float mTextSize = 0.0f;
        uint32_t mTextColor = 0;
        uint32_t mTextShadowColor = 0;
        uint32_t mFilterParams = 0;
        time_t mTimestamp = 0;
        uint32_t mDanmakuId = 0;
        int mMeasureFlag = 0;
        shared_ptr<Renderable> mRenderable;
        bool mSkipped = false;
    };

    typedef xl::RefPtr<BaseDanmaku> DanmakuRef;

}

#endif // _WTF_BASE_DANMAKU_HPP