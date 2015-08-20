#include <mutex>
#include "Displayer.hpp"
#include "DanmakusManager.hpp"
#include "WinmmTimer.hpp"
#include "Controller.hpp"

namespace WTFDanmaku {
    
    Controller::Controller() {

    }

    Controller::~Controller() {
        Stop();
    }

    void Controller::Initialize(void* hwnd) {
        mHwnd = hwnd;
        mTimer = WinmmTimer::Create();
        mManager = std::make_unique<DanmakusManager>();
        mManager->SetTimer(mTimer);

        mDisplayer = std::make_unique<Displayer>();
        mDisplayer->SetTarget(mHwnd);
    }

    bool Controller::HasCommands() {
        std::lock_guard<Win32Mutex> locker(mCommandQueueMutex);
        bool result = !mCommandQueue.empty();
        return result;
    }

    void Controller::PushCommand(const Command& cmd) {
        std::lock_guard<Win32Mutex> locker(mCommandQueueMutex);
        mCommandQueue.push(cmd);
    }

    Controller::Command Controller::PopCommand() {
        std::lock_guard<Win32Mutex> locker(mCommandQueueMutex);
        Command cmd = mCommandQueue.front();
        mCommandQueue.pop();
        return cmd;
    }

    DanmakusManager* Controller::GetManager() {
        return mManager.get();
    }

    bool Controller::IsRunning() {
        return mStatus == State::kRunning;
    }

    Controller::State Controller::GetState() {
        return mStatus;
    }

    void Controller::Start() {
        if (mStatus == State::kRunning) {
            // ignore
        } else if (mStatus == State::kPaused) {
            Resume();
        } else {
            if (mWorker.joinable()) {
                mWorker.detach();
            }
            mWorker = std::thread(&Controller::Working, this);
        }
    }
    
    void Controller::Pause() {
        if (mStatus == State::kRunning) {
            PushCommand(Cmd::kPause);
        }
    }

    void Controller::Resume() {
        if (mStatus == State::kPaused) {
            std::unique_lock<std::mutex> locker(mConditionMutex);
            mStatus = State::kRunning;
            PushCommand(Cmd::kResume);
            mCondition.notify_all();
        }
    }

    void Controller::Stop() {
        if (mStatus == State::kRunning || mStatus == State::kPaused) {
            if (mStatus == State::kPaused) {
                Resume();
            }

            PushCommand(Cmd::kStop);
            if (mWorker.joinable()) {
                mWorker.join();
            }
        }
    }

    void Controller::SeekTo(time_t milliseconds) {
        if (mStatus == State::kRunning || mStatus == State::kPaused) {
            Command cmd(Cmd::kSeek);
            cmd.arg1 = *reinterpret_cast<int*>(&milliseconds);
            cmd.arg2 = *(reinterpret_cast<int*>(&milliseconds) + 1);
            PushCommand(cmd);
        }
    }

    time_t Controller::GetCurrentPosition() {
        return mTimer->GetMilliseconds();
    }

    void Controller::HandleCommand() {
        if (!HasCommands())
            return;

        while (HasCommands()) {
            Command cmd = PopCommand();

            switch (cmd.what) {
                case Cmd::kResume:
                    mTimer->Resume();
                    mStatus = State::kRunning;
                    break;
                case Cmd::kPause:
                    if (mStatus == State::kRunning) {
                        mTimer->Pause();
                        std::unique_lock<std::mutex> locker(mConditionMutex);
                        mStatus = State::kPaused;
                        while (mStatus == State::kPaused) {
                            mCondition.wait(locker);
                        }
                    }
                    break;
                case Cmd::kSeek:
                    if (mStatus == State::kRunning) {
                        time_t timepoint = 0;
                        *reinterpret_cast<int*>(&timepoint) = cmd.arg1;
                        *(reinterpret_cast<int*>(&timepoint) + 1) = cmd.arg2;
                        mManager->SeekTo(timepoint);
                    }
                    break;
                case Cmd::kStop:
                    mStatus = State::kStopped;
                    break;
            }
        }
    }

    void Controller::Working() {
        mStatus = State::kRunning;
        bool succeed = mDisplayer->SetupBackend();
        if (!succeed) {
            OutputDebugStringW(L"mDisplayer->SetupBackend() failed");
            DebugBreak();
        }
        mTimer->Start();

        RenderingStatistics statistics;

        while (mStatus == State::kRunning || mStatus == State::kPaused) {
            HandleCommand();
            if (mStatus == State::kPaused) {
                std::this_thread::yield();
                continue;
            } else if (mStatus == State::kStopped) {
                break;
            }

            statistics = mManager->DrawDanmakus(mDisplayer.get());
            if (FAILED(statistics.lastHr))
                DebugBreak();
            // wait for vblank?
        }

        mTimer->Stop();
        mDisplayer->TeardownBackend();
    }

}