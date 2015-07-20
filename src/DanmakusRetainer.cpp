#include <cmath>
#include <map>
#include <set>
#include "BaseDanmaku.hpp"
#include "Displayer.hpp"
#include "DanmakusRetainer.hpp"

namespace DCDanmaku {

    typedef bool(*CompareFunction)(const int, const int);
    typedef std::map<int, DanmakuRef, std::less<int>> Danmakus;
    
    void DanmakusRetainer::Add(DanmakuRef danmaku, Displayer* displayer) {
        switch (danmaku->GetType()) {
            case DanmakuType::kScrolling:
                if (mR2LRetainer == nullptr) {
                    mR2LRetainer = std::move(unique_ptr<IDanmakusRetainer>(CreateR2LRetainer()));
                }
                mR2LRetainer->Add(danmaku, displayer);
                break;
            case DanmakuType::kTop:
                if (mTopRetainer == nullptr) {
                    mTopRetainer = std::move(unique_ptr<IDanmakusRetainer>(CreateTopRetainer()));
                }
                mTopRetainer->Add(danmaku, displayer);
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
        
    static bool CompareByYPos(const int item1, const int item2) {
        return item1 < item2;
    }

    static bool CompareByYPosDesc(const int item1, const int item2) {
        return item1 > item2;
    }

    class DanmakusRetainer::R2LRetainer : public IDanmakusRetainer {
    public:
        virtual void Add(DanmakuRef danmaku, Displayer* displayer) override {
            if (nullptr == mDanmakus) {
                mDanmakus = std::make_unique<Danmakus>();
            }
            
            time_t currentTime = displayer->GetTime();
            RemoveTimeoutDanmakus(currentTime);
            
            if (!danmaku->IsAlive(currentTime)) {
                return;
            }

            float top = 0.0f;

            for (auto iter = mDanmakus->begin(); iter != mDanmakus->end(); ++iter) {
                DanmakuRef item = iter->second;
                Rect<float> itemRect = item->GetRect();

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
                    time_t meetDuration = static_cast<time_t>((item->GetSpeed() * (currentTime - item->GetStartTime()) - item->GetWidth()) / speedDiff);
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
                    return;
                }
            }

            danmaku->Layout(displayer, static_cast<float>(displayer->GetWidth()), top);
            mDanmakus->insert(std::make_pair(static_cast<int>(top), danmaku));
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
            mDanmakus.release();
        }
    private:
        unique_ptr<Danmakus> mDanmakus;
    };

    IDanmakusRetainer* DanmakusRetainer::CreateR2LRetainer() {
        return new R2LRetainer();
    }

    class DanmakusRetainer::TopRetainer : public IDanmakusRetainer {
    public:
        virtual void Add(DanmakuRef danmaku, Displayer* displayer) override {
            if (nullptr == mDanmakus) {
                mDanmakus = std::make_unique<Danmakus>();
            }

            time_t currentTime = displayer->GetTime();
            RemoveTimeoutDanmakus(currentTime);

            if (!danmaku->IsAlive(currentTime)) {
                return;
            }

            float top = 0.0f;

            for (auto iter = mDanmakus->begin(); iter != mDanmakus->end(); ++iter) {
                DanmakuRef item = iter->second;
                Rect<float> itemRect = item->GetRect();

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
            mDanmakus->insert(std::make_pair(static_cast<int>(top), danmaku));
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

}