#include "Displayer.hpp"
#include "BottomDanmaku.hpp"
#include "DanmakusRetainer.hpp"

namespace WTFDanmaku {

    BottomDanmaku::~BottomDanmaku() {

    }

    DanmakuType BottomDanmaku::GetType() {
        return kBottom;
    }

    class BottomDanmaku::BottomRetainer : public IDanmakusRetainer {
    public:
        virtual ~BottomRetainer() override = default;

        virtual void Add(DanmakuRef danmaku, Displayer* displayer, DanmakuConfig* config, time_t currentMillis) override {
            if (nullptr == mDanmakus) {
                mDanmakus = std::make_unique<DecDanmakus>();
            }

            RemoveTimeoutDanmakus(currentMillis);

            if (!danmaku->IsAlive(currentMillis)) {
                return;
            }

            float screenBottom = static_cast<float>(displayer->GetHeight() - 1);
            float bottom = screenBottom;

            for (auto iter = mDanmakus->begin(); iter != mDanmakus->end(); ++iter) {
                DanmakuRef item = iter->second;
                Rect<float> itemRect = item->GetRect();

                if (item.Get() == danmaku.Get()) {
                    return;
                }

                if (itemRect.bottom < bottom) {
                    if (danmaku->GetHeight() <= bottom - itemRect.bottom) {
                        break;
                    }
                }

                bottom = itemRect.top - 1.0f;
                if (bottom - danmaku->GetHeight() < 0.0f) {
                    bottom = screenBottom;
                    break;
                }
            }
            float top = bottom - danmaku->GetHeight();
            danmaku->Layout(displayer, config, 0.0f, top);

            int bottomInt = static_cast<int>(bottom);
            auto iter = mDanmakus->find(bottomInt);
            if (iter != mDanmakus->end()) {
                iter->second = danmaku;
            } else {
                mDanmakus->insert(std::make_pair(bottomInt, danmaku));
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
        unique_ptr<DecDanmakus> mDanmakus;
    };

    std::unique_ptr<IDanmakusRetainer> BottomDanmaku::CreateRetainer() {
        return std::unique_ptr<IDanmakusRetainer>(new BottomRetainer);
    }

}