libwtfdanmaku
=============
High-performance danmaku engine for Windows, based on Direct2D and DirectComposition API.

Still under developing and is not stable for now. Support Windows 8+ for current.

### Features
- Hardware accelerated by Direct2D, DirectWrite, Direct3D 10.1 and DXGI 1.3
- Few cpu usage (<= 5%)
- Accurate danmaku layout

### TODO
- Renderable's bitmap invalid issue
- Multiple instance
- Rendering Statistics
- Further performance optimization
- Resize support (need rebuild device-dependent resources)
- BottomDanmaku, L2RDanmaku, Mode7 support, etc.
- Refined error handling
- User-friendly C & C++ API
- C++/CLI wrapper for .NET
- Optimize Renderable building (maybe concurrent)
- Windows Phone / UWP compatible
- Windows 7 compatible (maybe LayeredWindow?)
- Move to Direct3D 11 (?)
- Extensible architecture