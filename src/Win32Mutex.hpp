#ifndef _DCD_WIN32_MUTEX_HPP
#define _DCD_WIN32_MUTEX_HPP

#include <Windows.h>
#include "Noncopyable.hpp"

namespace DCDanmaku {

    class Win32Mutex : public Noncopyable {
    public:
        inline explicit Win32Mutex() {
            InitializeCriticalSection(&cs);
        }
        
        inline ~Win32Mutex() {
            DeleteCriticalSection(&cs);
        }

        inline void lock() {
            EnterCriticalSection(&cs);
        }

        inline void unlock() {
            LeaveCriticalSection(&cs);
        }
    private:
        CRITICAL_SECTION cs;
    };

}
#endif // _DCD_WIN32_MUTEX_HPP