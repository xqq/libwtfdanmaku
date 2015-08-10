#include "GlobalConfig.hpp"

namespace WTFDanmaku {

    bool sTopVisible = true;
    bool sR2LVisible = true;

    int sDanmakuDuration = 3800;

    int sLogicalScreenWidth = 682;
    int sLogicalScreenHeight = 438;

    float sFontScaleFactor = 1.0f;
    wchar_t* sFontName = L"SimHei";
    DWRITE_FONT_WEIGHT sFontWeight = DWRITE_FONT_WEIGHT_BOLD;
    DWRITE_FONT_STYLE sFontStyle = DWRITE_FONT_STYLE_NORMAL;
    DWRITE_FONT_STRETCH sFontStretch = DWRITE_FONT_STRETCH_NORMAL;

}