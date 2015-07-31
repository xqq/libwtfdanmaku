#include <dwrite.h>
#include "GlobalConfig.hpp"
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

    bool BaseDanmaku::HasMeasured() {
        return nullptr != mRenderable && mRenderable->HasTextLayout();
    }

    void BaseDanmaku::Measure(Displayer* displayer) {
        if (nullptr == mRenderable) {
            mRenderable = std::make_shared<Renderable>(this);
        }

        if (!mRenderable->HasTextLayout()) {
            // TODO
            ComPtr<IDWriteFactory> dwFactory = nullptr; //DM::GetInstance()->GetDWriteFactory();
            mRenderable->BuildTextLayout(dwFactory);
        }
    }
    
    void BaseDanmaku::ReleaseResources() {
        this->ReleaseRenderable();
    }

    bool BaseDanmaku::IsAlive(time_t time) {
        time_t elapsed = time - this->mStartTime;
        return elapsed >= 0 && elapsed <= sDanmakuDuration;
    }

    weak_ptr<Renderable> BaseDanmaku::BuildRenderable() {
        // TODO
        return mRenderable;
    }

}