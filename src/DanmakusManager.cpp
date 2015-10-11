#include <mutex>
#include "Displayer.hpp"
#include "DanmakusRetainer.hpp"
#include "DanmakusManager.hpp"

namespace WTFDanmaku { 

    bool DanmakusManager::TimeComparator::operator() (const DanmakuRef& a, const DanmakuRef& b) {
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

    DanmakusManager::DanmakusManager() {
        mNextFetchIter = mAllDanmakus.begin();
    }

    DanmakusManager::~DanmakusManager() {
        ReleaseActiveResources();
        mRetainer.Release();
        mActiveDanmakus.clear();
        mAllDanmakus.clear();
    }

    void DanmakusManager::SetDanmakuList(unique_ptr<std::vector<DanmakuRef>> danmakuArray) {
        std::lock_guard<Win32Mutex> locker(mAllDanmakusMutex);
        if (!mAllDanmakus.empty()) {
            mAllDanmakus.clear();
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
            if (diff < 0) {    // seek back, reset cached iterator
                mActiveDanmakus.clear();
                mNextFetchIter = mAllDanmakus.begin();
                mForceFetch = true;
            }
        }
    }

    void DanmakusManager::AddDanmaku(DanmakuRef danmaku) {
        std::lock_guard<Win32Mutex> locker(mAllDanmakusMutex);
        mAllDanmakus.insert(danmaku);
    }

    void DanmakusManager::AddLiveDanmaku(DanmakuRef danmaku) {
        std::lock_guard<Win32Mutex> locker(mActiveDanmakusMutex);
        if (danmaku->GetStartTime() == 0) {
            danmaku->SetStartTime(mTimer->GetMilliseconds() + 50);
        }
        mActiveDanmakus.insert(danmaku);
    }

    void DanmakusManager::FetchNewDanmakus(Displayer* displayer) {
        std::lock_guard<Win32Mutex> locker1(mAllDanmakusMutex);
        std::lock_guard<Win32Mutex> locker2(mActiveDanmakusMutex);

        time_t current = mTimer->GetMilliseconds();
        
        for (auto iter = mNextFetchIter; iter != mAllDanmakus.end(); /* ignore */) {
            if ((*iter)->GetStartTime() < current) {
                if (!(*iter)->HasMeasured(&mConfig)) {
                    (*iter)->Measure(displayer, &mConfig);
                }
                if ((*iter)->IsAlive(current)) {
                    mActiveDanmakus.insert(*iter);
                }
            } else {
                mNextFetchIter = iter;
                break;
            }

            if (++iter == mAllDanmakus.end()) {
                mNextFetchIter = mAllDanmakus.end();
                break;
            }
        }
        
        mLastFetchTime = current;
    }

    void DanmakusManager::RemoveTimeoutDanmakus() {
        std::lock_guard<Win32Mutex> locker(mActiveDanmakusMutex);

        time_t current = mTimer->GetMilliseconds();

        for (auto iter = mActiveDanmakus.begin(); iter != mActiveDanmakus.end(); /* ignore */) {
            if ((*iter)->GetStartTime() < current && !(*iter)->IsAlive(current)) {
                (*iter)->ReleaseResources();
                iter = mActiveDanmakus.erase(iter);
            } else {
                ++iter;
            }
        }
    }

    void DanmakusManager::ReleaseActiveResources() {
        std::lock_guard<Win32Mutex> locker(mActiveDanmakusMutex);

        mRetainer.Clear();

        for (auto iter = mActiveDanmakus.begin(); iter != mActiveDanmakus.end(); /* ignore*/) {
            (*iter)->ReleaseResources();
            iter = mActiveDanmakus.erase(iter);
        }
    }

    void DanmakusManager::ReLayout() {
        std::lock_guard<Win32Mutex> locker(mActiveDanmakusMutex);

        mConfig.LayoutFlag++;
        mRetainer.Clear();
    }

    bool DanmakusManager::IsVisibleDanmakuType(DanmakuRef danmaku, DanmakuConfig* config) {
        bool visible = true;

        if (danmaku->GetType() == DanmakuType::kScrolling) {
            visible = config->R2LVisible;
        } else if (danmaku->GetType() == DanmakuType::kTop) {
            visible = config->TopVisible;
        } else if (danmaku->GetType() == DanmakuType::kBottom) {
            visible = config->BottomVisible;
        }

        return visible;
    }

    RenderingStatistics DanmakusManager::DrawDanmakus(Displayer* displayer) {
        mTimer->Update();
        time_t current = mTimer->GetMilliseconds();
        
        if (mForceFetch || current - mLastFetchTime >= 100) {
            RemoveTimeoutDanmakus();
            FetchNewDanmakus(displayer);
            mForceFetch = false;
        }

        int count = 0;

        /* synchronized */
        {
            std::lock_guard<Win32Mutex> locker(mActiveDanmakusMutex);

            displayer->BeginDraw();

            for (auto iter = mActiveDanmakus.begin(); iter != mActiveDanmakus.end(); ++iter, ++count) {
                if (!(*iter)->HasMeasured(&mConfig)) {
                    (*iter)->Measure(displayer, &mConfig);
                }
                if (!(*iter)->HasLayout(&mConfig)) {
                    mRetainer.Add(*iter, displayer, &mConfig, current);
                }
                if (IsVisibleDanmakuType(*iter, &mConfig)) {
                    displayer->DrawDanmakuItem(*iter, current, &mConfig);
                }
            }
        }

        HRESULT hr = displayer->EndDraw();
        mStatistics.lastHr = hr;
        mStatistics.lastFrameDanmakuCount = count;
        mStatistics.lastFrameTime = current;

        return mStatistics;
    }

    RenderingStatistics DanmakusManager::GetRenderingStatistics() {
        return mStatistics;
    }

    DanmakuConfig* DanmakusManager::GetConfig() {
        return &mConfig;
    }

}