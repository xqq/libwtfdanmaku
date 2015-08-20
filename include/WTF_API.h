#ifndef _WTF_API_H
#define _WTF_API_H

#ifdef LIBWTFDANMAKU_EXPORTS
    #define WTF_API __declspec(dllexport)
#else
    #define WTF_API __declspec(dllimport)
#endif

#ifdef __cplusplus
    #define WTF_C_API extern "C" WTF_API
#else
    #define WTF_C_API WTF_API
#endif

#endif // _WTF_API_H