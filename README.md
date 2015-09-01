libwtfdanmaku
=============
High-performance danmaku engine for Windows, based on Direct2D and DirectComposition API.

Still under developing and only support Windows 8 or later now.

### Features
- Hardware accelerated by Direct2D, DirectWrite, Direct3D 11.0 and DXGI 1.2
- Few cpu usage (<= 5%) with smooth animation
- Accurate danmaku layout

### TODO
- ~~Renderable's bitmap invalid issue~~
- ~~Multiple instance~~
- ~~x64 compatible~~
- Optimize danmaku style effect
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
- ~~Move to Direct3D 11~~
- Extensible architecture

### Build
My environment: Visual Studio Community 2013. Also tested under VS2015.

Don't forget to checkout submodule (git submodule init, git submodule update, ...)

Launch WTFDanmaku.sln for demo and libwtfdanmaku library.

libwtfdanmaku is permitted to work under shared library (DLL) according to the license.

### Screenshot

![5cmps](https://raw.githubusercontent.com/xqq/xqq.github.io/master/img/wtf_screenshot_1.jpg)

### License
Copyright (C) 2015 xqq <xqq@0ginr.com>

Licensed under LGPL v2.1