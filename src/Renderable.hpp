#ifndef _WTF_RENDERABLE_HPP
#define _WTF_RENDERABLE_HPP

#include <cstdint>
#include <wrl.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include "Noncopyable.hpp"

using Microsoft::WRL::ComPtr;

namespace WTFDanmaku {

    class Displayer;
    class BaseDanmaku;
    struct DanmakuConfig;

    class Renderable : public Noncopyable {
    public:
        explicit Renderable(BaseDanmaku* danmaku) : mDanmaku(danmaku) { }

        ~Renderable() = default;

        inline bool HasTextLayout() {
            return mTextLayout.Get() != nullptr;
        }

        inline ComPtr<IDWriteTextLayout> GetTextLayout() {
            return mTextLayout;
        }

        bool BuildTextLayout(Displayer* displayer, DanmakuConfig* config);

        bool HasBitmap(DanmakuConfig* config);

        inline ComPtr<ID2D1Bitmap1> GetBitmap() {
            return mBitmap;
        }

        bool BuildBitmap(Displayer* displayer, DanmakuConfig* config);

        inline void Release() {
            mTextLayout.Reset();
            mBitmap.Reset();
        }
    private:
        BaseDanmaku* mDanmaku = nullptr;
        ComPtr<IDWriteTextLayout> mTextLayout = nullptr;
        ComPtr<ID2D1Bitmap1> mBitmap = nullptr;
        int mBitmapValidFlag = -1;
    };

}

#endif // _WTF_RENDERABLE_HPP