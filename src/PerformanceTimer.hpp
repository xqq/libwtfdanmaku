#ifndef _WTF_PERFORMANCE_TIMER_HPP
#define _WTF_PERFORMANCE_TIMER_HPP

#include "ITimer.hpp"

namespace WTFDanmaku {

    typedef union _LargeInteger {
        struct {
            uint32_t LowPart;
            int32_t HighPart;
        };
        int64_t QuadPart;
    } LargeInteger;

    class PerformanceTimer : public ITimer {
    public:
        static inline TimerRef Create() {
            return std::make_shared<PerformanceTimer>();
        }
        static time_t GetGlobalCurrent();
    public:
        PerformanceTimer() = default;
        virtual ~PerformanceTimer() override = default;
        virtual void Start() override;
        virtual void Pause() override;
        virtual void Resume() override;
        virtual ITimer* Update() override;
        virtual void Stop() override;
        virtual void AddOffset(int64_t offset) override;
        virtual time_t GetMilliseconds() override;
    private:
        LargeInteger mFrequency;
        LargeInteger mBeginCounter;
        int64_t mTimeBase = 0;
        int64_t mCurrent = 0;
    };

}

#endif // _WTF_PERFORMANCE_TIMER_HPP
