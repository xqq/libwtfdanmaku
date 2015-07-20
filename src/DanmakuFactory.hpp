#ifndef _DCD_DANMAKU_FACTORY_HPP
#define _DCD_DANMAKU_FACTORY_HPP

#include <cstdint>
#include <string>
#include <memory>
#include "Noncopyable.hpp"
#include "BaseDanmaku.hpp"

using std::shared_ptr;

namespace DCDanmaku {

    class DanmakuFactory : public Noncopyable {
    public:
        static DanmakuRef CreateDanmaku(DanmakuType type, time_t time, std::wstring& comment, int fontSize, int fontColor, time_t timestamp = 0, int danmakuId = 0);
    };

}

#endif // _DCD_DANMAKU_FACTORY_HPP