#ifndef _WTF_WIN32_MUTEX_HPP
#define _WTF_WIN32_MUTEX_HPP

#include <Windows.h>
#include "Noncopyable.hpp"

namespace WTFDanmaku {

    class Win32Mutex : public Noncopyable {
    public:
        inline Win32Mutex() {
            InitializeCriticalSectionEx(&cs, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
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
#endif // _WTF_WIN32_MUTEX_HPP