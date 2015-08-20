#ifndef _WTF_ENGINE_HPP
#define _WTF_ENGINE_HPP

#include <cstdint>
#include "WTF_API.h"

namespace WTFDanmaku {

    class Controller;

    enum Type : int {
        Scrolling = 1,
        Bottom = 4,
        Top = 5,
        Reverse = 6,
        Position = 7,
        Advanced = 8
    };

    class WTF_API WTFEngine {
    public:
        explicit WTFEngine();
        ~WTFEngine();
        void Initialize(void* hwnd);
        void LoadBilibiliFile(const char* filePath);
        void LoadBilibiliXml(const char* str);
        void AddDanmaku(Type type, time_t time, const wchar_t* comment, int fontSize, int fontColor, time_t timestamp = 0, int danmakuId = 0);
        void AddLiveDanmaku(Type type, time_t time, const wchar_t* comment, int fontSize, int fontColor, time_t timestamp = 0, int danmakuId = 0);
        void Start();
        void Pause();
        void Resume();
        void Stop();
        void SeekTo(time_t milliseconds);
        time_t GetCurrentPosition();
        bool IsRunning();
    private:
        WTFEngine(const WTFEngine&) = delete;
        WTFEngine& operator=(const WTFEngine&) = delete;
    private:
        Controller* mController;
    };

}


#endif // _WTF_ENGINE_HPP