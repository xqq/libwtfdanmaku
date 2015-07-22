#ifndef _WTF_DANMAKUS_MANAGER_HPP
#define _WTF_DANMAKUS_MANAGER_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include <set>
#include "BaseDanmaku.hpp"
#include "Noncopyable.hpp"
#include "ITimer.hpp"

using std::unique_ptr;

namespace WTFDanmaku {

    class Displayer;

    class DanmakusManager : public Noncopyable {
    public:
        void SetDanmakuList(unique_ptr<std::vector<DanmakuRef>> danmakuArray);
        void SetTimer(TimerRef timer);
        void AddDanmaku(DanmakuRef danmaku);
        void AddLiveDanmaku(DanmakuRef danmaku);
        void DrawDanmakus(Displayer* displayer);
    private:
        void FetchNewDanmakus();
        void RemoveTimeoutDanmakus();
    private:
        struct TimeComparator;
        typedef std::set<DanmakuRef, TimeComparator> TimeSortedDanmakus;
    private:
        TimerRef mTimer;
        time_t mLastFetchTime = 0;
        unique_ptr<TimeSortedDanmakus> mAllDanmakus;
        unique_ptr<TimeSortedDanmakus> mActiveDanmakus;
    };

}

#endif // _WTF_DANMAKUS_MANAGER_HPP