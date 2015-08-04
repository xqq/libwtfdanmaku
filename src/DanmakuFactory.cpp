#include "R2LDanmaku.hpp"
#include "TopDanmaku.hpp"
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
            case kNull:
            default:
                return nullptr;
        }

        danmaku->mStartTime = time;
        danmaku->mTimestamp = timestamp;
        danmaku->mDanmakuId = danmakuId;
        danmaku->mComment = comment;
        danmaku->mTextSize = static_cast<float>(fontSize);
        danmaku->mTextColor = fontColor | 0xFF000000;
        danmaku->mTextShadowColor = (danmaku->mTextColor <= 0xFF000000) ? 0xFFFFFFFF : 0xFF000000;

        return danmaku;
    }

}