#include <Windows.h>
#include "PerformanceTimer.hpp"

namespace WTFDanmaku {

    time_t PerformanceTimer::GetGlobalCurrent() {
        LARGE_INTEGER frequency, current;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&current);
        return current.QuadPart * 1000 / frequency.QuadPart;
    }

    void PerformanceTimer::Start() {
        mTimeBase = 0;
        Resume();
    }

    void PerformanceTimer::Pause() {
        LARGE_INTEGER current;
        QueryPerformanceCounter(&current);
        int64_t milliseconds = (current.QuadPart - mBeginCounter.QuadPart) * 1000 / mFrequency.QuadPart;
        mTimeBase += milliseconds;
    }

    void PerformanceTimer::Resume() {
        QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&mFrequency));
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&mBeginCounter));
    }

    ITimer* PerformanceTimer::Update() {
        LARGE_INTEGER current;
        QueryPerformanceCounter(&current);
        int64_t milliseconds = (current.QuadPart - mBeginCounter.QuadPart) * 1000 / mFrequency.QuadPart;
        mCurrent = milliseconds + mTimeBase;
        return this;
    }

    void PerformanceTimer::Stop() {
        mBeginCounter.QuadPart = 0;
        mTimeBase = 0;
    }

    void PerformanceTimer::AddOffset(int64_t offset) {
        mTimeBase += offset;
    }

    time_t PerformanceTimer::GetMilliseconds() {
        return mCurrent;
    }

}