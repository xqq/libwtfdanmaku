#include <mutex>
#include "Displayer.hpp"
#include "DanmakusRetainer.hpp"
#include "DanmakusManager.hpp"

namespace WTFDanmaku { 

    bool DanmakusManager::TimeComparator::operator() (DanmakuRef a, DanmakuRef b) {
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

    void DanmakusManager::SetDanmakuList(unique_ptr<std::vector<DanmakuRef>> danmakuArray) {
        std::lock_guard<Win32Mutex> locker(mAllDanmakusMutex);
        if (!mAllDanmakus.empty()) {
            return;
        }
        for (auto iter = danmakuArray->begin(); iter != danmakuArray->end(); ++iter) {
            mAllDanmakus.insert(*iter);
        }
        mNextFetchIter = mAllDanmakus.begin();
    }

    void DanmakusManager::SetTimer(TimerRef timer) {
        mTimer = timer;
    }

    void DanmakusManager::SeekTo(time_t timepoint) {
        std::lock_guard<Win32Mutex> locker(mActiveDanmakusMutex);

        time_t current = mTimer->GetMilliseconds();
        int64_t diff = timepoint - current;

        if (diff != 0) {
            mTimer->AddOffset(diff);
            mActiveDanmakus.clear();
            if (diff < 0) {    // seek back, reset cached iterator
                mNextFetchIter = mAllDanmakus.begin();
            }
        }
    }

    void DanmakusManager::AddDanmaku(DanmakuRef danmaku) {
        std::lock_guard<Win32Mutex> locker(mAllDanmakusMutex);
        mAllDanmakus.insert(danmaku);
    }

    void DanmakusManager::AddLiveDanmaku(DanmakuRef danmaku) {
        std::lock_guard<Win32Mutex> locker(mActiveDanmakusMutex);
        mActiveDanmakus.insert(danmaku);
    }

    void DanmakusManager::FetchNewDanmakus() {
        std::lock_guard<Win32Mutex> locker1(mAllDanmakusMutex);
        std::lock_guard<Win32Mutex> locker2(mActiveDanmakusMutex);

        time_t current = mTimer->GetMilliseconds();
        
        for (auto iter = mNextFetchIter; iter != mAllDanmakus.end(); ++iter) {
            if ((*iter)->GetStartTime() < current) {
                if ((*iter)->IsAlive(current)) {
                    mActiveDanmakus.insert(*iter);
                }
            } else {
                mNextFetchIter = iter;
                break;
            }
        }
        
        mLastFetchTime = current;
    }

    void DanmakusManager::RemoveTimeoutDanmakus() {
        std::lock_guard<Win32Mutex> locker(mActiveDanmakusMutex);

        time_t current = mTimer->GetMilliseconds();
        for (auto iter = mActiveDanmakus.begin(); iter != mActiveDanmakus.end(); /* ignore*/) {
            if (!(*iter)->IsAlive(current)) {
                (*iter)->ReleaseResources();
                iter = mActiveDanmakus.erase(iter);
            } else {
                ++iter;
            }
        }
    }

    void DanmakusManager::DrawDanmakus(Displayer* displayer) {
        mTimer->Update();
        time_t current = mTimer->GetMilliseconds();
        
        if (current - mLastFetchTime >= 100) {
            RemoveTimeoutDanmakus();
            FetchNewDanmakus();
        }

        std::lock_guard<Win32Mutex> locker(mActiveDanmakusMutex);

        auto iter = mActiveDanmakus.begin();
        if (iter == mActiveDanmakus.end())
            return;

        displayer->BeginDraw();

        for (/* iter */; iter != mActiveDanmakus.end(); ++iter) {
            if (!(*iter)->HasMeasured()) {
                (*iter)->Measure(displayer);
            }
            if (!(*iter)->HasLayout()) {
                mRetainer.Add(*iter, displayer, current);
            }
            displayer->DrawDanmakuItem(*iter, current);
        }

        HRESULT hr = displayer->EndDraw();
        // TODO: handle hr
    }

}