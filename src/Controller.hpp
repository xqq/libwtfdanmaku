#ifndef _WTF_CONTROLLER_HPP
#define _WTF_CONTROLLER_HPP

#include <cstdint>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "BaseDanmaku.hpp"
#include "Win32Mutex.hpp"
#include "Noncopyable.hpp"
#include "ITimer.hpp"

namespace WTFDanmaku {

    class Displayer;
    class DanmakusManager;

    class Controller : public Noncopyable {
    public:
        enum class State : int {
            kIdle = 0,
            kRunning = 1,
            kPaused = 2,
            kStopped = 3
        };
    private:
        enum class Cmd : int {
            kNull = 0,
            kBase = 0x12450,
            kStart = kBase + 1,
            kPause = kBase + 2,
            kResume = kBase + 3,
            kSeek = kBase + 4,
            kStop = kBase + 5,
            kResize = kBase + 6,
            kReLayout = kBase + 7,
            kSetDpi = kBase + 8
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
        int Initialize(void* hwnd, uint32_t initialWidth = 0, uint32_t initialHeight = 0);
        void Terminate();
        int QuerySwapChain(const void* pGuid, void** ppObject);
        DanmakusManager* GetManager();
        void Start();
        void Pause();
        void Resume();
        void Stop();
        void SeekTo(time_t milliseconds);
        void Resize(uint32_t width, uint32_t height);
        void SetDpi(uint32_t dpiX, uint32_t dpiY);
        void ReLayout();
        time_t GetCurrentPosition();
        bool IsRunning();
        State GetState();
    private:
        void Working();
        bool HasCommands();
        void PushCommand(const Command& cmd);
        Command PopCommand();
        void HandleCommand();
    private:
        State mStatus = State::kIdle;
        std::thread mWorker;
        std::mutex mConditionMutex;
        std::condition_variable mCondition;
        void* mHwnd = nullptr;
        bool mHasBackend = false;
        TimerRef mTimer;
        Win32Mutex mCommandQueueMutex;
        std::queue<Command> mCommandQueue;
        std::unique_ptr<Displayer> mDisplayer;
        std::unique_ptr<DanmakusManager> mManager;
    };

}

#endif // _WTF_CONTROLLER_HPP