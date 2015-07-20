#ifndef _DCD_DANMAKUS_MANAGER_HPP
#define _DCD_DANMAKUS_MANAGER_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include <set>
#include "BaseDanmaku.hpp"
#include "Noncopyable.hpp"

using std::unique_ptr;

namespace DCDanmaku {

    class DanmakusManager : public Noncopyable {
    public:
        void SetDanmakuList(unique_ptr<std::vector<DanmakuRef>> danmakuArray);
        void AddDanmaku(DanmakuRef danmaku);
    private:
        struct TimeComparator;
        typedef std::set<DanmakuRef, TimeComparator> TimeSortedDanmakus;
    private:
        unique_ptr<TimeSortedDanmakus> mAllDanmakus;
        unique_ptr<TimeSortedDanmakus> mActiveDanmakus;
        time_t mLastFetchTime = 0;

    };

}

#endif // _DCD_DANMAKUS_MANAGER_HPP