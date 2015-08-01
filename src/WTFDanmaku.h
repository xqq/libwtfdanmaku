#ifdef LIBWTFDANMAKU_EXPORTS
#define WTF_APT __declspec(dllexport)
#else
#define WTF_APT __declspec(dllimport)
#endif

WTF_APT int fuck(void);

WTF_APT int shit(void);