#include "Displayer.hpp"
#include "TopDanmaku.hpp"
#include "DanmakuConfig.hpp"
#include "DanmakusRetainer.hpp"

namespace WTFDanmaku {

    TopDanmaku::~TopDanmaku() {

    }

    DanmakuType TopDanmaku::GetType() {
        return kTop;
    }

    void TopDanmaku::Layout(Displayer* displayer, DanmakuConfig* config, float x, float y) {
        this->y = y;
        mRect.top = y;
        mLayoutFlag = config->LayoutFlag;
    }

    float TopDanmaku::GetSpeed() {
        return 0.0f;
    }

    float TopDanmaku::GetLeftAtTime(Displayer* displayer, time_t time) {
        float screenWidth = static_cast<float>(displayer->GetWidth());

        return screenWidth / 2 - mTextWidth / 2;
    }

    Rect<float> TopDanmaku::GetRectAtTime(Displayer* displayer, time_t time) {
        mRect.left = GetLeftAtTime(displayer, time);
        mRect.right = mRect.left + mTextWidth;
        mRect.top = y;
        mRect.bottom = mRect.top + mTextHeight;

        return mRect;
    }

    class TopDanmaku::TopRetainer : public IDanmakusRetainer {
    public:
        virtual ~TopRetainer() override = default;

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

                top = itemRect.bottom + 1.0f;
                if (top + danmaku->GetHeight() > displayer->GetHeight()) {
                    top = 0.0f;
                    break;
                }
            }

            danmaku->Layout(displayer, config, 0.0f, top);
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

    std::unique_ptr<IDanmakusRetainer> TopDanmaku::CreateRetainer() {
        return std::unique_ptr<IDanmakusRetainer>(new TopRetainer);
    }

}