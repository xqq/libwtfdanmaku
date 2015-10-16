#ifndef _WTF_DANMAKUS_MANAGER_HPP
#define _WTF_DANMAKUS_MANAGER_HPP

#include <cstdint>
#include <set>
#include <vector>
#include <memory>
#include "BaseDanmaku.hpp"
#include "Noncopyable.hpp"
#include "DanmakusRetainer.hpp"
#include "DanmakuConfig.hpp"
#include "Win32Mutex.hpp"
#include "ITimer.hpp"

using std::unique_ptr;

namespace WTFDanmaku {

    class Displayer;
    class DanmakusRetainer;

    struct RenderingStatistics {
    public:
        float fps = 0.0f;
        time_t lastFrameTime = 0;
        int lastFrameDanmakuCount = 0;
        HRESULT lastHr = NULL;
    };

    class DanmakusManager : public Noncopyable {
    public:
        explicit DanmakusManager();
        ~DanmakusManager();
        void SetDanmakuList(unique_ptr<std::vector<DanmakuRef>> danmakuArray);
        void SetTimer(TimerRef timer);
        void SeekTo(time_t timepoint);
        void AddDanmaku(DanmakuRef danmaku);
        void AddLiveDanmaku(DanmakuRef danmaku);
        void ReleaseActiveResources();
        void ReLayout();
        DanmakuConfig* GetConfig();
        RenderingStatistics DrawDanmakus(Displayer* displayer);
        RenderingStatistics GetRenderingStatistics();
    private:
        void FetchNewDanmakus(Displayer* displayer);
        void RemoveTimeoutDanmakus();
        void PrebuildRenderableTask(Displayer* displayer, time_t thisFrameTime, time_t remainTime);
        bool IsVisibleDanmakuType(DanmakuRef danmaku, DanmakuConfig* config);
    private:
        struct TimeComparator {
            bool operator() (const DanmakuRef& a, const DanmakuRef& b);
        };
        typedef std::set<DanmakuRef, TimeComparator> TimeSortedDanmakus;
    private:
        TimerRef mTimer;
        bool mForceFetch = false;
        bool mInPrebuildProgress = false;
        time_t mLastFetchTime = 0;
        int mPrebuildBitmapValidFlag = -1;
        TimeSortedDanmakus::iterator mNextFetchIter;
        TimeSortedDanmakus::iterator mNextPrebuildIter;
        Win32Mutex mAllDanmakusMutex;
        Win32Mutex mActiveDanmakusMutex;
        TimeSortedDanmakus mAllDanmakus;
        TimeSortedDanmakus mActiveDanmakus;
        DanmakusRetainer mRetainer;
        DanmakuConfig mConfig;
        RenderingStatistics mStatistics;
    };

}

#endif // _WTF_DANMAKUS_MANAGER_HPP