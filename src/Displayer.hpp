#ifndef _DCD_DISPLAYER_HPP
#define _DCD_DISPLAYER_HPP

#include <cstdint>
#include <memory>
#include "BaseDanmaku.hpp"
#include "Noncopyable.hpp"

using std::unique_ptr;

namespace DCDanmaku {

    class DisplayerImpl;

    class Displayer : public Noncopyable {
    public:
        int GetWidth();
        int GetHeight();
        float GetDpiX();
        float GetDpiY();
        void Resize(int width, int height);
        void DrawDanmakuItem(DanmakuRef item);
    private:
        unique_ptr<DisplayerImpl> pImpl;
    };

}

#endif // _DCD_DISPLAYER_HPP