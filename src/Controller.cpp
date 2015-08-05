#include <mutex>
#include "Displayer.hpp"
#include "DanmakusManager.hpp"
#include "WinmmTimer.hpp"
#include "Controller.hpp"

namespace WTFDanmaku {
    
    Controller::Controller() {

    }

    Controller::~Controller() {

    }

    void Controller::Initialize(void* windowHandle) {
        mHwnd = windowHandle;
        mTimer = WinmmTimer::Create();
        mManager = std::make_unique<DanmakusManager>();
        mManager->SetTimer(mTimer);

        mDisplayer = std::make_unique<Displayer>();
        mDisplayer->SetTarget(mHwnd);
    }

    void Controller::Initialize(void* windowHandle, unique_ptr<std::vector<DanmakuRef>> danmakuArray) {
        Initialize(windowHandle);
        mManager->SetDanmakuList(std::move(danmakuArray));
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

    void Controller::AddDanmaku(DanmakuRef danmaku) {
        mManager->AddDanmaku(danmaku);
    }
    void Controller::AddLiveDanmaku(DanmakuRef danmaku) {
        mManager->AddLiveDanmaku(danmaku);
    }

    bool Controller::IsRunning() {
        return mCurrentState == kInitializing || mCurrentState == kRunning;
    }

    void Controller::Start() { // TODO
        if (IsRunning()) {
            PushCommand(Cmd::kStart);
        } else {
            mTimer->Start();
        }
    }
    
    void Controller::Pause() {  // TODO
        if (IsRunning()) {
            PushCommand(Cmd::kPause);
        }
    }

    void Controller::Resume() {
        if (mCurrentState == kPause) {

        }
    }

    void Controller::Stop() {
        if (IsRunning()) {
            PushCommand(Cmd::kStop);
        }
    }

    void Controller::SeekTo(time_t milliseconds) {
        if (IsRunning()) {
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
                case Cmd::kStart:

                    break;
                case Cmd::kPause:

                    break;
                case Cmd::kResume:

                    break;
                case Cmd::kSeek:

                    break;
                case Cmd::kStop:

                    break;
            }
        }
    }

    void Controller::Working() {
        bool succeed = mDisplayer->SetupBackend();
        if (!succeed) {
            // error handling?
            OutputDebugStringW(L"mDisplayer->SetupBackend() failed");
        }

        while (mCurrentState == kRunning) {
            HandleCommand();

            mManager->DrawDanmakus(mDisplayer.get());
            // wait for vblank?
        }
    }

}