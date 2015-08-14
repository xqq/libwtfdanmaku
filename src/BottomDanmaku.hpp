#ifndef _WTF_BOTTOM_DANMAKU_HPP
#define _WTF_BOTTOM_DANMAKU_HPP

#include "TopDanmaku.hpp"

namespace WTFDanmaku {

    class BottomDanmaku : public TopDanmaku {
    public:
        static inline DanmakuRef Create() {
            return std::make_shared<BottomDanmaku>();
        }
    public:
        explicit BottomDanmaku() = default;
        virtual ~BottomDanmaku() override;
        virtual DanmakuType GetType() override;
    };

}


#endif // _WTF_BOTTOM_DANMAKU_HPP