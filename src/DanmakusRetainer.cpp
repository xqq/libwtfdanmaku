#include <cmath>
#include <map>
#include <functional>
#include "BaseDanmaku.hpp"
#include "Displayer.hpp"
#include "DanmakusRetainer.hpp"

namespace WTFDanmaku {

    typedef bool(*CompareFunction)(const int, const int);
    typedef std::map<int, DanmakuRef, std::less<int>> Danmakus;
    
    void DanmakusRetainer::Add(DanmakuRef danmaku, Displayer* displayer, time_t currentMillis) {
        switch (danmaku->GetType()) {
            case DanmakuType::kScrolling:
                if (mR2LRetainer == nullptr) {
                    mR2LRetainer = std::move(unique_ptr<IDanmakusRetainer>(CreateR2LRetainer()));
                }
                mR2LRetainer->Add(danmaku, displayer, currentMillis);
                break;
            case DanmakuType::kTop:
                if (mTopRetainer == nullptr) {
                    mTopRetainer = std::move(unique_ptr<IDanmakusRetainer>(CreateTopRetainer()));
                }
                mTopRetainer->Add(danmaku, displayer, currentMillis);
                break;
            case DanmakuType::kBottom:
                if (mBottomRetainer == nullptr) {
                    mBottomRetainer = std::move(unique_ptr<IDanmakusRetainer>(CreateBottomRetainer()));
                }
                mBottomRetainer->Add(danmaku, displayer, currentMillis);
                break;
            default:
                break;
        }
    }

    void DanmakusRetainer::Clear() {
        if (mR2LRetainer != nullptr)
            mR2LRetainer->Clear();
        if (mTopRetainer != nullptr)
            mTopRetainer->Clear();
    }

    void DanmakusRetainer::Release() {
        Clear();
        mR2LRetainer.reset();
        mTopRetainer.reset();
    }

    class DanmakusRetainer::R2LRetainer : public IDanmakusRetainer {
    public:
        virtual void Add(DanmakuRef danmaku, Displayer* displayer, time_t currentMillis) override {
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

                if (item.get() == danmaku.get()) {
                    return;
                }

                if (itemRect.top > top) {
                    if (danmaku->GetHeight() <= itemRect.top - top) {
                        break;
                    }
                }

                bool suitable = false;
                float speedDiff = danmaku->GetSpeed() - item->GetSpeed();

                if (speedDiff <= 0) {
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
                    danmaku->Layout(displayer, static_cast<float>(displayer->GetWidth()), -danmaku->GetHeight() - 1);
                    return;
                }
            }

            danmaku->Layout(displayer, static_cast<float>(displayer->GetWidth()), top);
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

    IDanmakusRetainer* DanmakusRetainer::CreateR2LRetainer() {
        return new R2LRetainer();
    }

    class DanmakusRetainer::TopRetainer : public IDanmakusRetainer {
    public:
        virtual void Add(DanmakuRef danmaku, Displayer* displayer, time_t currentMillis) override {
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

                if (item.get() == danmaku.get()) {
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
            
            danmaku->Layout(displayer, 0.0f, top);
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

    IDanmakusRetainer* DanmakusRetainer::CreateTopRetainer() {
        return new TopRetainer();
    }

    class DanmakusRetainer::BottomRetainer : public IDanmakusRetainer {
    private:
        typedef std::map<int, DanmakuRef, std::greater<int>> DecDanmakus;
    public:
        virtual void Add(DanmakuRef danmaku, Displayer* displayer, time_t currentMillis) override {
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

                if (item.get() == danmaku.get()) {
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
            danmaku->Layout(displayer, 0.0f, top);

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

    IDanmakusRetainer* DanmakusRetainer::CreateBottomRetainer() {
        return new BottomRetainer();
    }

}