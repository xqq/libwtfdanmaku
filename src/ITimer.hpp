#ifndef _WTF_TIMER_HPP
#define _WTF_TIMER_HPP

#include <cstdint>
#include <memory>
#include "Noncopyable.hpp"

using std::shared_ptr;

namespace WTFDanmaku {

    class ITimer : public Noncopyable {
    public:
        ITimer() = default;
        virtual ~ITimer() = default;
        virtual void Start() = 0;
        virtual void Pause() = 0;
        virtual void Resume() = 0;
        virtual ITimer* Update() = 0;
        virtual void Stop() = 0;
        virtual void AddOffset(int64_t offset) = 0;
        virtual time_t GetMilliseconds() = 0;
    };

    typedef shared_ptr<ITimer> TimerRef;

}

#endif // _WTF_TIMER_HPP