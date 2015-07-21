#ifndef _DCD_DISPLAYER_HPP
#define _DCD_DISPLAYER_HPP

#include <cstdint>
#include "ITimer.hpp"
#include "BaseDanmaku.hpp"
#include "Noncopyable.hpp"

namespace DCDanmaku {

    class Displayer : public Noncopyable {
    public:
        int GetWidth();
        int GetHeight();
        float GetDpiX();
        float GetDpiY();
        void Resize(int width, int height);
        void DrawDanmakuItem(DanmakuRef item);
    private:
        TimerRef mTimer;
    };

}

#endif // _DCD_DISPLAYER_HPP