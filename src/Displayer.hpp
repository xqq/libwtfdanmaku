#ifndef _DCD_DISPLAYER_HPP
#define _DCD_DISPLAYER_HPP

#include <cstdint>
#include "Noncopyable.hpp"

namespace DCDanmaku {

    class Displayer : public Noncopyable {
    public:
        int GetWidth();
        int GetHeight();
        float GetDpiX();
        float GetDpiY();
        void Resize(int width, int height);
        time_t GetTime();
    private:

    };

}

#endif // _DCD_DISPLAYER_HPP