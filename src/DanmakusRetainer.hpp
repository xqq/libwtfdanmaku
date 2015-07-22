#ifndef _WTF_DANMAKU_RETAINER_HPP
#define _WTF_DANMAKU_RETAINER_HPP

#include <cstdint>
#include <memory>
#include "Noncopyable.hpp"
#include "BaseDanmaku.hpp"

using std::unique_ptr;

namespace WTFDanmaku {

    class Displayer;

    class IDanmakusRetainer : public Noncopyable {
    public:
        virtual void Add(DanmakuRef danmaku, Displayer* displayer, time_t currentMillis) = 0;
        virtual void Clear() = 0;
        virtual void Release() = 0;
    };

    class DanmakusRetainer : public Noncopyable {
    public:
        void Add(DanmakuRef danmaku, Displayer* displayer, time_t currentMillis);
        void Clear();
        void Release();
    private:
        IDanmakusRetainer* CreateR2LRetainer();
        IDanmakusRetainer* CreateTopRetainer();
    private:
        class R2LRetainer;
        class TopRetainer;
    private:
        unique_ptr<IDanmakusRetainer> mR2LRetainer = nullptr;
        unique_ptr<IDanmakusRetainer> mTopRetainer = nullptr;
    };

}

#endif // _WTF_DANMAKU_RETAINER_HPP