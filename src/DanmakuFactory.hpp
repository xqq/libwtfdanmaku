#ifndef _WTF_DANMAKU_FACTORY_HPP
#define _WTF_DANMAKU_FACTORY_HPP

#include <cstdint>
#include <string>
#include <memory>
#include "Noncopyable.hpp"
#include "BaseDanmaku.hpp"

using std::shared_ptr;

namespace WTFDanmaku {

    class DanmakuFactory : public Noncopyable {
    public:
        static DanmakuRef CreateDanmaku(DanmakuType type, time_t time, std::wstring& comment, int fontSize, int fontColor, time_t timestamp = 0, int danmakuId = 0);
    private:
        static void ReplaceStringInplace(std::wstring& str, const wchar_t* search, const wchar_t* replace);
    };

}

#endif // _WTF_DANMAKU_FACTORY_HPP