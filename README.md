libwtfdanmaku
=============
High-performance danmaku engine for Windows, based on Direct2D and DirectComposition API.

This project is still under developing.

### Features
- Hardware accelerated by Direct2D, DirectWrite, Direct3D 11.0 and DXGI 1.2
- Supports Windows 8 or later, Windows 8.1 Store App, and Windows 7 with [Platform Update](https://msdn.microsoft.com/en-us/library/windows/desktop/jj863687(v=vs.85).aspx)
- Few cpu usage (<= 5%) with smooth animation
- Accurate danmaku layout

### TODO
- ~~Renderable's bitmap invalid issue~~
- ~~Multiple instance~~
- ~~x64 compatible~~
- Optimize danmaku style effect
- Rendering Statistics
- ~~Hi-DPI support, and Pre-monitor DPI Aware~~
- Further performance optimization
- ~~Resize support (need rebuild device-dependent resources)~~
- ~~BottomDanmaku~~, L2RDanmaku, Mode7 support, etc.
- Refined error handling
- User-friendly ~~C & C++ API~~
- C++/CLI wrapper for .NET
- Interoperate with Media Player
- ~~Optimize Renderable building~~
- ~~Windows Phone / UWP compatible~~
- ~~Windows 7 compatible (maybe LayeredWindow?)~~
- ~~Move to Direct3D 11~~
- Extensible architecture

### Build
My environment: Visual Studio Community 2013. Also tested under VS2015.

Don't forget to checkout submodule (git submodule init, git submodule update, ...)

Launch WTFDanmaku.sln for demo and libwtfdanmaku library.

libwtfdanmaku is permitted to work under shared library (DLL) according to the license.

### Build for Win7 or Windows Store
First, enter the patches directory:
```bash
cd patches
```
To build for Windows 7 (with Platform Update):
```bash
git am 0002-vcxproj-Build-for-Windows7.patch
```
or build for Windows 8.1 store app:
```bash
git am 0003-vcxproj-Build-for-Windows8.1-store-app.patch
```

### Screenshot

![5cmps](https://raw.githubusercontent.com/xqq/xqq.github.io/master/img/wtf_screenshot_1.jpg)

### License
Copyright (C) 2015 xqq <xqq@0ginr.com>

Licensed under LGPL v2.1