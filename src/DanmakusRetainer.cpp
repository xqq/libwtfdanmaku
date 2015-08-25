#include <cmath>
#include <map>
#include <functional>
#include "BaseDanmaku.hpp"
#include "R2LDanmaku.hpp"
#include "TopDanmaku.hpp"
#include "BottomDanmaku.hpp"
#include "Displayer.hpp"
#include "DanmakusRetainer.hpp"

namespace WTFDanmaku {

    void DanmakusRetainer::Add(DanmakuRef danmaku, Displayer* displayer, DanmakuConfig* config, time_t currentMillis) {
        switch (danmaku->GetType()) {
            case DanmakuType::kScrolling:
                if (mR2LRetainer == nullptr) {
                    mR2LRetainer = R2LDanmaku::CreateRetainer();
                }
                mR2LRetainer->Add(danmaku, displayer, config, currentMillis);
                break;
            case DanmakuType::kTop:
                if (mTopRetainer == nullptr) {
                    mTopRetainer = TopDanmaku::CreateRetainer();
                }
                mTopRetainer->Add(danmaku, displayer, config, currentMillis);
                break;
            case DanmakuType::kBottom:
                if (mBottomRetainer == nullptr) {
                    mBottomRetainer = BottomDanmaku::CreateRetainer();
                }
                mBottomRetainer->Add(danmaku, displayer, config, currentMillis);
                break;
            default:
                break;
        }
    }

    void DanmakusRetainer::Clear() {
        if (mR2LRetainer != nullptr)
            mR2LRetainer->Clear();
        if (mTopRetainer != nullptr)
            mTopRetainer->Clear();
        if (mBottomRetainer != nullptr)
            mBottomRetainer->Clear();
    }

    void DanmakusRetainer::Release() {
        Clear();
        mR2LRetainer.reset();
        mTopRetainer.reset();
        mBottomRetainer.reset();
    }

}