#ifdef LIBDCDANMAKU_EXPORTS
#define DCD_API __declspec(dllexport)
#else
#define DCD_API __declspec(dllimport)
#endif

DCD_API int fuck(void);

DCD_API int shit(void);