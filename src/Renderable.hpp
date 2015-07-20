#ifndef _DCD_RENDERABLE_HPP
#define _DCD_RENDERABLE_HPP

#include <cstdint>
#include <wrl.h>
#include <d2d1_2.h>
#include <dwrite.h>
#include "Noncopyable.hpp"

using namespace Microsoft::WRL;

namespace DCDanmaku {

    class BaseDanmaku;

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

        bool BuildTextLayout(ComPtr<IDWriteFactory> dwFactory);

        inline bool HasBitmap() {
            return mBitmap.Get() != nullptr;
        }

        inline ComPtr<ID2D1Bitmap> GetBitmap() {
            return mBitmap;
        }

        bool BuildBitmap();

        inline void Release() {
            mTextLayout.Reset();
            mBitmap.Reset();
        }
    private:
        BaseDanmaku* mDanmaku = nullptr;
        ComPtr<IDWriteTextLayout> mTextLayout = nullptr;
        ComPtr<ID2D1Bitmap> mBitmap = nullptr;
    };

}

#endif // _DCD_RENDERABLE_HPP