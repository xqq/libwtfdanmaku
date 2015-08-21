#include "WinmmTimer.hpp"
#include "R2LDanmaku.hpp"
#include "TopDanmaku.hpp"
#include "BottomDanmaku.hpp"
#include "DanmakuFactory.hpp"

namespace WTFDanmaku {

    DanmakuRef DanmakuFactory::CreateDanmaku(DanmakuType type, time_t time, std::wstring& comment, int fontSize, int fontColor, time_t timestamp, int danmakuId) {
        DanmakuRef danmaku = nullptr;

        switch (type) {
            case kScrolling:
                danmaku = R2LDanmaku::Create();
                break;
            case kTop:
                danmaku = TopDanmaku::Create();
                break;
            case kBottom:
                danmaku = BottomDanmaku::Create();
                break;
            case kNull:
            default:
                return nullptr;
        }

        if (timestamp == 0) {
            timestamp = WinmmTimer::GetGlobalCurrent();
        }

        danmaku->mStartTime = time;
        danmaku->mTimestamp = timestamp;
        danmaku->mDanmakuId = danmakuId;
        danmaku->mComment = comment;
        ReplaceStringInplace(danmaku->mComment, L"/n", L"\r\n");
        danmaku->mTextSize = static_cast<float>(fontSize);
        danmaku->mTextColor = fontColor | 0xFF000000;
        danmaku->mTextShadowColor = (danmaku->mTextColor <= 0xFF000000) ? 0xFFFFFFFF : 0xFF000000;

        return danmaku;
    }

    void DanmakuFactory::ReplaceStringInplace(std::wstring& str, const wchar_t* search, const wchar_t* replace) {
        size_t searchLength = wcslen(search);
        size_t replaceLength = wcslen(replace);

        size_t pos = 0;
        while ((pos = str.find(search, pos)) != std::wstring::npos) {
            str.replace(pos, searchLength, replace);
            pos += replaceLength;
        }
    }

}