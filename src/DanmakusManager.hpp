#ifndef _WTF_DANMAKUS_MANAGER_HPP
#define _WTF_DANMAKUS_MANAGER_HPP

#include <cstdint>
#include <set>
#include <vector>
#include <memory>
#include "BaseDanmaku.hpp"
#include "Noncopyable.hpp"
#include "DanmakusRetainer.hpp"
#include "Win32Mutex.hpp"
#include "ITimer.hpp"

using std::unique_ptr;

namespace WTFDanmaku {

    class Displayer;
    class DanmakusRetainer;

    class DanmakusManager : public Noncopyable {
    public:
        explicit DanmakusManager();
        void SetDanmakuList(unique_ptr<std::vector<DanmakuRef>> danmakuArray);
        void SetTimer(TimerRef timer);
        void SeekTo(time_t timepoint);
        void AddDanmaku(DanmakuRef danmaku);
        void AddLiveDanmaku(DanmakuRef danmaku);
        void DrawDanmakus(Displayer* displayer);
    private:
        void FetchNewDanmakus();
        void RemoveTimeoutDanmakus();
    private:
        struct TimeComparator {
            bool operator() (DanmakuRef a, DanmakuRef b);
        };
        typedef std::set<DanmakuRef, TimeComparator> TimeSortedDanmakus;
    private:
        TimerRef mTimer;
        time_t mLastFetchTime = 0;
        TimeSortedDanmakus::iterator mNextFetchIter;
        Win32Mutex mAllDanmakusMutex;
        Win32Mutex mActiveDanmakusMutex;
        TimeSortedDanmakus mAllDanmakus;
        TimeSortedDanmakus mActiveDanmakus;
        DanmakusRetainer mRetainer;
    };

}

#endif // _WTF_DANMAKUS_MANAGER_HPP