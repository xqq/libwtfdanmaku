#ifndef _DCD_GLOBAL_CONFIG_HPP
#define _DCD_GLOBAL_CONFIG_HPP

#include <cstdint>
#include <dwrite.h>
#include "Noncopyable.hpp"

namespace DCDanmaku {

    extern bool sTopVisible;
    extern bool sR2LVisible;

    extern int sDanmakuDuration;

    extern int sLogicalScreenWidth;
    extern int sLogicalScreenHeight;

    extern float sFontScaleFactor;

    extern wchar_t* sFontName;
    extern DWRITE_FONT_WEIGHT sFontWeight;
    extern DWRITE_FONT_STYLE sFontStyle;
    extern DWRITE_FONT_STRETCH sFontStretch;

}

#endif // _DCD_GLOBAL_CONFIG_HPP