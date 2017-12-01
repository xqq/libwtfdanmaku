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
- R2LDanmaku lines limitation
- Keywords filter by regular expression
- C++/CX wrapper for C#/WinRT invoking
- More smooth animation (percision problem when calculating coordinates?)
- ~~Refactor project files to vcxitems (Shared/Windows/WindowsPhone)~~

### Build
My environment: Visual Studio Community 2013. Also tested under VS2015.

Don't forget to checkout submodule (git submodule update --init)

- Win32 Desktop
 - Import project `libwtfdanmaku.Desktop`
- Win32 Desktop (for Windows7 compatibility)
 - Import project `libwtfdanmaku.Windows7`
- Windows 10 Universal Platform
 - Import project `libwtfdanmaku.Universal`
- Windows 8.1 WinRT/Universal
 - Import project `libwtfdanmaku.Windows`
- Windows Phone 8.1
 - Import project `libwtfdanmaku.WindowsPhone`

libwtfdanmaku is permitted to work under shared library (DLL) according to the license.

### Screenshot

![5cmps](https://raw.githubusercontent.com/xqq/xqq.github.io/master/img/wtf_screenshot_1.jpg)

### License
Copyright (C) 2015 xqq <xqq@0ginr.com>

Licensed under LGPL v2.1