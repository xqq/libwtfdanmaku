#include <mutex>
#include "Displayer.hpp"
#include "DanmakusRetainer.hpp"
#include "DanmakusManager.hpp"
#include "Renderable.hpp"

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
        mNextPrebuildIter = mAllDanmakus.begin();
        mPrebuildBitmapValidFlag = mConfig.BitmapValidFlag;
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
        mNextPrebuildIter = mAllDanmakus.begin();
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
                mNextPrebuildIter = mAllDanmakus.begin();
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
                (*iter)->SetSkipped(false);
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

    void DanmakusManager::PrebuildRenderableTask(Displayer* displayer, time_t thisFrameTime, time_t remainTime) {
        if (mAllDanmakus.empty() || mNextPrebuildIter == mAllDanmakus.end()) {
            return;
        }

        if (mPrebuildBitmapValidFlag != mConfig.BitmapValidFlag) {
            mNextPrebuildIter = mAllDanmakus.begin();
            mInPrebuildProgress = true;
            mPrebuildBitmapValidFlag = mConfig.BitmapValidFlag;
        }

        int64_t timepoint = thisFrameTime % 10000;  // clamp time to [0ms, 9999ms], that is 0s ~ 10s
        time_t frameTimeBase = thisFrameTime - timepoint;
        if (mInPrebuildProgress == false) {
            if ((thisFrameTime >= 0 && thisFrameTime < 2000) || (timepoint >= 7000 && timepoint < 9000)) {
                mInPrebuildProgress = true;
            } else {
                return;
            }
        }

        std::lock_guard<Win32Mutex> locker(mAllDanmakusMutex);

        const int kCurrentSection = 0;
        const int kNextSection = 1;
        int section = 0;

        if (timepoint < 7000) {    // Task: Build cache for current time section
            section = kCurrentSection;
            time_t itemStartTime = (*mNextPrebuildIter)->GetStartTime();
            if (itemStartTime >= frameTimeBase + 10000) {    // Is it in next section?
                mInPrebuildProgress = false;    // Current task has been completed. Just go back.
                return;
            }
        } else if (timepoint >= 7000 && timepoint < 9999) {    // Task: Build cache for next 10-seconds time section
            section = kNextSection;
            time_t itemStartTime = (*mNextPrebuildIter)->GetStartTime();
            if (itemStartTime >= frameTimeBase + 10000 + 10000) {
                mInPrebuildProgress = false;
                return;
            }
        }

        time_t beginTime = mTimer->GetMilliseconds();

        for (auto iter = mNextPrebuildIter; iter != mAllDanmakus.end(); /* ignore */) {
            time_t itemStartTime = (*iter)->GetStartTime();

            if (itemStartTime >= thisFrameTime) {
                if (section == kCurrentSection) {
                    if (itemStartTime >= frameTimeBase + 10000) {
                        mNextPrebuildIter = iter;
                        mInPrebuildProgress = false;
                        break;
                    }
                } else if (section == kNextSection) {
                    if (itemStartTime >= frameTimeBase + 10000 + 10000) {
                        mNextPrebuildIter = iter;
                        mInPrebuildProgress = false;
                        break;
                    }
                }

                if (!(*iter)->HasMeasured(&mConfig))
                    (*iter)->Measure(displayer, &mConfig);
                auto renderable = (*iter)->GetRenderable().lock();
                if (!renderable->HasBitmap(&mConfig)) {
                    renderable->BuildBitmap(displayer, &mConfig);
                } else {
                    ++iter; continue;
                }
            } else if (itemStartTime < thisFrameTime) {    // seeked iterator?
                if (!(*iter)->HasMeasured(&mConfig)) {
                    (*iter)->Measure(displayer, &mConfig);
                }
                if ((*iter)->IsAlive(thisFrameTime)) {
                    auto renderable = (*iter)->GetRenderable().lock();
                    if (!renderable->HasBitmap(&mConfig)) {
                        renderable->BuildBitmap(displayer, &mConfig);
                    } else {
                        ++iter; continue;
                    }
                } else {
                    ++iter; continue;    // fast-forward to thisFrameTime
                }
            }

            time_t now = mTimer->Update()->GetMilliseconds();
            if (now - beginTime >= remainTime - 1) {
                mNextPrebuildIter = ++iter;    // remain time excceed, save iterator and exit.
                break;
            }

            if (++iter == mAllDanmakus.end()) {
                mNextPrebuildIter = mAllDanmakus.end();
                break;
            }
        }
    }

    bool DanmakusManager::IsVisibleDanmakuType(DanmakuRef danmaku, DanmakuConfig* config) {
        if (danmaku->IsSkipped()) {
            return false;
        }

        DanmakuType type = danmaku->GetType();

        if (type == DanmakuType::kScrolling) {
            return config->R2LVisible;
        } else if (type == DanmakuType::kTop) {
            return config->TopVisible;
        } else if (type == DanmakuType::kBottom) {
            return config->BottomVisible;
        }

        return true;
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
        HRESULT hr = S_OK;

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

            hr = displayer->EndDraw();

            time_t now = mTimer->Update()->GetMilliseconds();
            int64_t elapsed = now - current;
            if (elapsed < 14) {
                time_t remain = 14 - elapsed;
                PrebuildRenderableTask(displayer, current, remain);
            }
        }

        hr = displayer->Present();

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