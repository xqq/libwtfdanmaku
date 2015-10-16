#include <mutex>
#include "Displayer.hpp"
#include "DanmakusManager.hpp"
#include "WinmmTimer.hpp"
#include "PerformanceTimer.hpp"
#include "Controller.hpp"

namespace WTFDanmaku {
    
    Controller::Controller() {

    }

    Controller::~Controller() {
        Stop();
        Terminate();
    }

    int Controller::Initialize(void* hwnd, uint32_t initialWidth, uint32_t initialHeight) {
        if (hwnd == NULL && (initialWidth == 0 || initialHeight == 0))
            return -1;

        mHwnd = hwnd;
#ifdef _WTF_BUILD_UWP
        mTimer = PerformanceTimer::Create();
#else
        mTimer = WinmmTimer::Create();
#endif
        mManager = std::make_unique<DanmakusManager>();
        mManager->SetTimer(mTimer);

        mDisplayer = std::make_unique<Displayer>();
        mDisplayer->SetTarget(mHwnd, initialWidth, initialHeight);

        bool succeed = mDisplayer->SetupBackend();
        if (!succeed) {
            mHasBackend = false;
            return -1;
        }
        mHasBackend = true;
        return 0;
    }

    void Controller::Terminate() {
        if (mHasBackend) {
            mDisplayer->TeardownBackend();
            mHasBackend = false;
        }
    }

    int Controller::QuerySwapChain(const void* pGuid, void** ppObject) {
        if (mDisplayer == nullptr)
            return -1;

        return mDisplayer->QuerySwapChain(pGuid, ppObject);
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

    void Controller::Resize(uint32_t width, uint32_t height) {
        if (mStatus == State::kRunning || mStatus == State::kPaused) {
            Command cmd(Cmd::kResize);
            cmd.arg1 = width;
            cmd.arg2 = height;
            PushCommand(cmd);
        }
    }

    void Controller::SetDpi(uint32_t dpiX, uint32_t dpiY) {
        if (mStatus == State::kRunning || mStatus == State::kPaused) {
            Command cmd(Cmd::kSetDpi);
            cmd.arg1 = dpiX;
            cmd.arg2 = dpiY;
            PushCommand(cmd);
        } else {
            mDisplayer->SetDpi(dpiX, dpiY);
        }
    }

    void Controller::ReLayout() {
        if (mStatus == State::kRunning || mStatus == State::kPaused) {
            PushCommand(Cmd::kReLayout);
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
                case Cmd::kResize:
                    mDisplayer->Resize(cmd.arg1, cmd.arg2);
                    mManager->GetConfig()->MeasureFlag++;
                    break;
                case Cmd::kReLayout:
                    mManager->ReLayout();
                    break;
                case Cmd::kSetDpi: {
                    float nowDpiX = mDisplayer->GetDpiX();
                    float nowDpiY = mDisplayer->GetDpiY();
                    if (cmd.arg1 != static_cast<uint32_t>(nowDpiX) || cmd.arg2 != static_cast<uint32_t>(nowDpiY)) {
                        mDisplayer->SetDpi(cmd.arg1, cmd.arg2);
                        mManager->GetConfig()->MeasureFlag++;
                        mManager->GetConfig()->BitmapValidFlag++;
                        mManager->ReLayout();
                    }
                }
                    break;
                case Cmd::kStop:
                    mStatus = State::kStopped;
                    break;
            }
        }
    }

    void Controller::Working() {
#ifndef _WTF_BUILD_UWP
        if (!mHasBackend) DebugBreak();
#endif

        mStatus = State::kRunning;
        mTimer->Start();

        RenderingStatistics statistics;

        while (mStatus == State::kRunning || mStatus == State::kPaused) {
            HandleCommand();
            if (mStatus == State::kStopped) {
                break;
            }

            statistics = mManager->DrawDanmakus(mDisplayer.get());
            if (FAILED(statistics.lastHr)) {
                std::wstring message(L"Received HRESULT Error from DrawDanmakus, hr = ");
                message.append(std::to_wstring(statistics.lastHr));
                message.append(L"\n");
                OutputDebugStringW(message.c_str());
            }
        }

        mTimer->Stop();
        mManager->ReleaseActiveResources();
    }

}