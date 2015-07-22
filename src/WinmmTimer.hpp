#ifndef _WTF_WINMM_TIMER_HPP
#define _WTF_WINMM_TIMER_HPP

#include "ITimer.hpp"

namespace WTFDanmaku {

    class WinmmTimer : public ITimer {
    public:
        explicit WinmmTimer();
        virtual ~WinmmTimer() override;
        virtual void Start() override;
        virtual void Pause() override;
        virtual void Resume() override;
        virtual void Update() override;
        virtual void Increase(time_t offset) override;
        virtual void Decrease(time_t offset) override;
        virtual void Stop() override;
        virtual time_t GetMilliseconds() override;
    private:
        int64_t mBeginTime = 0;
        int64_t mTimeBase = 0;
        int64_t mCurrent = 0;
    };

}

#endif // _WTF_WINMM_TIMER_HPP