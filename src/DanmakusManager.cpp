#include "Displayer.hpp"
#include "DanmakusManager.hpp"

namespace WTFDanmaku { 

    struct DanmakusManager::TimeComparator {
        bool operator() (DanmakuRef a, DanmakuRef b) {
            int64_t diff = 0;

            diff = a->GetStartTime() - b->GetStartTime();
            if (diff < 0)
                return true;
            else if (diff > 0)
                return false;

            diff = a->GetSendTimestamp() - b->GetSendTimestamp();
            if (diff < 0)
                return true;
            else if (diff > 0)
                return false;

            diff = a->GetDanmakuId() - b->GetDanmakuId();
            if (diff < 0)
                return true;
            else if (diff > 0)
                return false;

            return true;
        }
    };

    void DanmakusManager::SetDanmakuList(unique_ptr<std::vector<DanmakuRef>> danmakuArray) {
        if (nullptr == mAllDanmakus) {
            mAllDanmakus = std::make_unique<TimeSortedDanmakus>();
        }
        if (!mAllDanmakus->empty()) {
            return;
        }
        for (auto iter = danmakuArray->begin(); iter != danmakuArray->end(); ++iter) {
            mAllDanmakus->insert(*iter);
        }

    }

    void DanmakusManager::SetTimer(TimerRef timer) {
        mTimer = timer;
    }

    void DanmakusManager::AddDanmaku(DanmakuRef danmaku) {

    }

    void DanmakusManager::AddLiveDanmaku(DanmakuRef danmaku) {
        mActiveDanmakus->insert(danmaku);
    }

    void DanmakusManager::FetchNewDanmakus() {

    }

    void DanmakusManager::RemoveTimeoutDanmakus() {
        for (auto iter = mActiveDanmakus->begin(); iter != mActiveDanmakus->end(); /* ignore*/) {
            if (!(*iter)->IsAlive(mTimer->GetMilliseconds())) {
                iter = mActiveDanmakus->erase(iter);
            } else {
                ++iter;
            }
        }
    }

    void DanmakusManager::DrawDanmakus(Displayer* displayer) {
        RemoveTimeoutDanmakus();
        FetchNewDanmakus();
        for (auto iter = mActiveDanmakus->begin(); iter != mActiveDanmakus->end(); ++iter) {
            displayer->DrawDanmakuItem(*iter);
        }
    }

}