#include <Windows.h>
#include "WinmmTimer.hpp"

#pragma comment (lib, "winmm.lib")

namespace DCDanmaku {

    WinmmTimer::WinmmTimer() {
        timeBeginPeriod(1);
    }

    WinmmTimer::~WinmmTimer() {
        timeEndPeriod(1);
    }

    void WinmmTimer::Start() {
        mTimeBase = 0;
        mBeginTime = timeGetTime();
    }

    void WinmmTimer::Pause() {
        mTimeBase += timeGetTime() - mBeginTime;
    }

    void WinmmTimer::Resume() {
        mBeginTime = timeGetTime();
    }

    void WinmmTimer::Update() {
        mCurrent = timeGetTime() - mBeginTime + mTimeBase;
    }

    void WinmmTimer::Increase(time_t offset) {
        mTimeBase += offset;
    }

    void WinmmTimer::Decrease(time_t offset) {
        mTimeBase -= offset;
    }

    void WinmmTimer::Stop() {
        mBeginTime = 0;
        mTimeBase = 0;
    }

    time_t WinmmTimer::GetMilliseconds() {
        return mCurrent;
    }

}