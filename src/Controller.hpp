#ifndef _WTF_CONTROLLER_HPP
#define _WTF_CONTROLLER_HPP

#include <cstdint>
#include <queue>
#include <memory>
#include <thread>
#include "Win32Mutex.hpp"
#include "Noncopyable.hpp"
#include "ITimer.hpp"

namespace WTFDanmaku {

    class Displayer;
    class DanmakusManager;

    class Controller : public Noncopyable {
    private:
        enum State : int {
            kIdle = 0,
            kInitializing = 1,
            kRunning = 2,
            kPaused = 3,
            kStopped = 4
        };

        enum Cmd : int {
            kNull = 0,
            kBase = 0x12450,
            kStart = kBase + 1,
            kPause = kBase + 2,
            kResume = kBase + 3,
            kSeek = kBase + 4,
            kStop = kBase + 5
        };

        struct Command {
        public:
            Cmd what = Cmd::kNull;
            int arg1 = 0;
            int arg2 = 0;
            Command() = default;
            Command(Cmd _what) : what(_what) {}
        };
    public:
        explicit Controller();
        ~Controller();
        void Initialize(void* windowHandle);
        void Initialize(void* windowHandle, unique_ptr<std::vector<DanmakuRef>> danmakuArray);
        void AddDanmaku(DanmakuRef danmaku);
        void AddLiveDanmaku(DanmakuRef danmaku);
        void Start();
        void Pause();
        void Resume();
        void Stop();
        void SeekTo(time_t milliseconds);
        time_t GetCurrentPosition();
        bool IsRunning();
    private:
        void Working();
        bool HasCommands();
        void PushCommand(const Command& cmd);
        Command PopCommand();
        void HandleCommand();
    private:
        int mCurrentState = State::kIdle;
        int mTargetState = State::kIdle;
        std::thread mWorker;
        void* mHwnd = nullptr;
        TimerRef mTimer;
        Win32Mutex mCommandQueueMutex;
        std::queue<Command> mCommandQueue;
        std::unique_ptr<Displayer> mDisplayer;
        std::unique_ptr<DanmakusManager> mManager;
    };

}

#endif // _WTF_CONTROLLER_HPP