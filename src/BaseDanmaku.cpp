#include <d2d1_1.h>
#include <dwrite.h>
#include "DanmakuConfig.hpp"
#include "Renderable.hpp"
#include "BaseDanmaku.hpp"

namespace WTFDanmaku {

    BaseDanmaku::BaseDanmaku() {
    
    }

    BaseDanmaku::~BaseDanmaku() {

    }

    DanmakuType BaseDanmaku::GetType() {
        return kNull;
    }

    bool BaseDanmaku::HasMeasured(DanmakuConfig* config) {
        return nullptr != mRenderable
            && mRenderable->HasTextLayout()
            && mMeasureFlag == config->MeasureFlag;
    }

    bool BaseDanmaku::HasLayout(DanmakuConfig* config) {
        return mLayoutFlag == config->LayoutFlag;
    }

    void BaseDanmaku::Measure(Displayer* displayer, DanmakuConfig* config) {
        if (nullptr == mRenderable) {
            mRenderable = std::make_shared<Renderable>(this);
        }

        mRenderable->BuildTextLayout(displayer, config);

        mDuration = config->DanmakuDuration;
        mMeasureFlag = config->MeasureFlag;
    }
    
    void BaseDanmaku::ReleaseResources() {
        this->ReleaseRenderable();
    }

    D2D1_MATRIX_4X4_F BaseDanmaku::GetPerspectiveTransformAtTime(Displayer* displayer, time_t time) {
        return D2D1::Matrix4x4F::PerspectiveProjection(0);
    }

    bool BaseDanmaku::IsAlive(time_t time) {
        time_t elapsed = time - this->mStartTime;
        return elapsed > 0 && elapsed <= mDuration;
    }

    weak_ptr<Renderable> BaseDanmaku::BuildRenderable(Displayer* displayer, DanmakuConfig* config) {
        if (nullptr == mRenderable || !mRenderable->HasTextLayout())
            this->Measure(displayer, config);

        if (!mRenderable->HasBitmap(config)) {
            mRenderable->BuildBitmap(displayer, config);
        }

        return mRenderable;
    }

}