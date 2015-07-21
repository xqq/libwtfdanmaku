#ifndef _DCD_TIMER_HPP
#define _DCD_TIMER_HPP

#include <cstdint>
#include <memory>
#include "Noncopyable.hpp"

using std::shared_ptr;

namespace DCDanmaku {

    class ITimer : public Noncopyable {
    public:
        explicit ITimer() = default;
        virtual ~ITimer() = default;
        virtual void Start() = 0;
        virtual void Pause() = 0;
        virtual void Resume() = 0;
        virtual void Update() = 0;
        virtual void Increase(time_t offset) = 0;
        virtual void Decrease(time_t offset) = 0;
        virtual void Stop() = 0;
        virtual time_t GetMilliseconds() = 0;
    };

    typedef shared_ptr<ITimer> TimerRef;

}

#endif // _DCD_TIMER_HPP