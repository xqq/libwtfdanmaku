#include "Displayer.hpp"
#include "R2LDanmaku.hpp"
#include "DanmakuConfig.hpp"
#include "DanmakusRetainer.hpp"

namespace WTFDanmaku {

    R2LDanmaku::~R2LDanmaku() {
        
    }

    DanmakuType R2LDanmaku::GetType() {
        return kScrolling;
    }

    void R2LDanmaku::Measure(Displayer* displayer, DanmakuConfig* config) {
        this->BaseDanmaku::Measure(displayer, config);
        float scaleFactor = displayer->GetDpiX() / 96.0f;
        mSpeed = (config->LogicalScreenWidth * scaleFactor + mTextWidth) / config->DanmakuDuration;
        int screenWidth = displayer->GetWidth();
        mDuration = static_cast<time_t>((screenWidth + mTextWidth) / mSpeed);
    }

    void R2LDanmaku::Layout(Displayer* displayer, DanmakuConfig* config, float x, float y) {
        this->y = y;
        mRect.top = y;
        mLayoutFlag = config->LayoutFlag;
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

    class R2LDanmaku::R2LRetainer : public IDanmakusRetainer {
    public:
        virtual ~R2LRetainer() override = default;

        virtual void Add(DanmakuRef danmaku, Displayer* displayer, DanmakuConfig* config, time_t currentMillis) override {
            if (nullptr == mDanmakus) {
                mDanmakus = std::make_unique<Danmakus>();
            }

            RemoveTimeoutDanmakus(currentMillis);

            if (!danmaku->IsAlive(currentMillis)) {
                return;
            }

            float top = 0.0f;

            for (auto iter = mDanmakus->begin(); iter != mDanmakus->end(); ++iter) {
                DanmakuRef item = iter->second;
                Rect<float> itemRect = item->GetRect();

                if (item.Get() == danmaku.Get()) {
                    return;
                }

                if (itemRect.top > top) {
                    if (danmaku->GetHeight() <= itemRect.top - top) {
                        break;
                    }
                }

                bool suitable = false;
                float speedDiff = danmaku->GetSpeed() - item->GetSpeed();

                if (danmaku->GetLeftAtTime(displayer, currentMillis) <= itemRect.right + 30) {
                    suitable = false;
                } else if (speedDiff <= 0) {
                    if (itemRect.right <= displayer->GetWidth() - 30) {
                        suitable = true;
                    }
                } else {
                    time_t meetDuration = static_cast<time_t>(
                        (item->GetSpeed() * (currentMillis - item->GetStartTime()) - item->GetWidth()) / speedDiff
                    );
                    if (meetDuration * danmaku->GetSpeed() >= displayer->GetWidth() + 30) {
                        suitable = true;
                    }
                }

                if (suitable) {
                    top = itemRect.top;
                    break;
                }

                top = itemRect.bottom + 1.0f;
                if (top + danmaku->GetHeight() > displayer->GetHeight()) {
                    danmaku->SetSkipped(true);
                    danmaku->Layout(displayer, config, static_cast<float>(displayer->GetWidth()), -danmaku->GetHeight() - 1);
                    return;
                }
            }

            danmaku->Layout(displayer, config, static_cast<float>(displayer->GetWidth()), top);
            int topint = static_cast<int>(top);
            auto iter = mDanmakus->find(topint);
            if (iter != mDanmakus->end()) {
                iter->second = danmaku;
            } else {
                mDanmakus->insert(std::make_pair(topint, danmaku));
            }
        }

        void RemoveTimeoutDanmakus(time_t time) {
            auto iter = mDanmakus->begin();
            while (iter != mDanmakus->end()) {
                if (iter->second->IsAlive(time) == false) {
                    iter = mDanmakus->erase(iter);
                } else {
                    ++iter;
                }
            }
        }

        virtual void Clear() override {
            mDanmakus->clear();
        }

        virtual void Release() override {
            mDanmakus.reset();
        }
    private:
        unique_ptr<Danmakus> mDanmakus;
    };

    std::unique_ptr<IDanmakusRetainer> R2LDanmaku::CreateRetainer() {
        return std::unique_ptr<IDanmakusRetainer>(new R2LRetainer);
    }

}