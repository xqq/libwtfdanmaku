libwtfdanmaku
=============
High-performance danmaku engine for Windows, based on Direct2D and DirectComposition API.

Still under developing and support Windows 8+ for now.

### Features
- Hardware accelerated by Direct2D, DirectWrite, Direct3D 10.1 and DXGI 1.3
- Few cpu usage (<= 5%) with smooth animation
- Accurate danmaku layout

### TODO
- ~~Renderable's bitmap invalid issue~~
- ~~Multiple instance~~
- Rendering Statistics
- ~~Hi-DPI support~~, and pre-Monitor DPI Aware
- Further performance optimization
- ~~Resize support (need rebuild device-dependent resources)~~
- ~~BottomDanmaku~~, L2RDanmaku, Mode7 support, etc.
- Refined error handling
- User-friendly ~~C & C++ API~~
- C++/CLI wrapper for .NET
- Interoperate with Media Player
- Optimize Renderable building (maybe concurrent)
- Windows Phone / UWP compatible
- Windows 7 compatible (maybe LayeredWindow?)
- Move to Direct3D 11 (?)
- Extensible architecture

### Build
My environment: Visual Studio Community 2013. Also tested under VS2015.

Don't forget to checkout submodule (git submodule init, git submodule update, ...)

Launch WTFDanmaku.sln for demo and libwtfdanmaku library.

libwtfdanmaku is both working for static library, or shared library (DLL). Modify vcxproj file on your own.