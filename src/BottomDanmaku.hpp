#ifndef _WTF_BOTTOM_DANMAKU_HPP
#define _WTF_BOTTOM_DANMAKU_HPP

#include "TopDanmaku.hpp"

namespace WTFDanmaku {

    class BottomDanmaku : public TopDanmaku {
    public:
        static inline DanmakuRef Create() {
            return xl::RefPtr<BottomDanmaku>(new BottomDanmaku);
        }
        static std::unique_ptr<IDanmakusRetainer> CreateRetainer();
    public:
        explicit BottomDanmaku() = default;
        virtual ~BottomDanmaku() override;
        virtual DanmakuType GetType() override;
    private:
        class BottomRetainer;
    };

}


#endif // _WTF_BOTTOM_DANMAKU_HPP