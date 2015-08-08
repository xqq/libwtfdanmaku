#ifdef LIBWTFDANMAKU_EXPORTS
#define WTF_API __declspec(dllexport)
#else
#define WTF_API __declspec(dllimport)
#endif

WTF_API int fuck(void);

WTF_API int shit(void);