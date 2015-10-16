#ifndef _WTF_WINMM_TIMER_HPP
#define _WTF_WINMM_TIMER_HPP

#include "ITimer.hpp"

#ifndef _WTF_BUILD_UWP

namespace WTFDanmaku {

    class WinmmTimer : public ITimer {
    public:
        static inline TimerRef Create() {
            return std::make_shared<WinmmTimer>();
        }
        static time_t GetGlobalCurrent();
    public:
        WinmmTimer();
        virtual ~WinmmTimer() override;
        virtual void Start() override;
        virtual void Pause() override;
        virtual void Resume() override;
        virtual ITimer* Update() override;
        virtual void Stop() override;
        virtual void AddOffset(int64_t offset) override;
        virtual time_t GetMilliseconds() override;
    private:
        int64_t mBeginTime = 0;
        int64_t mTimeBase = 0;
        int64_t mCurrent = 0;
    };

}

#endif // _WTF_BUILD_UWP

#endif // _WTF_WINMM_TIMER_HPP