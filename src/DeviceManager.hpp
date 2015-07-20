#ifndef _DCD_DEVICE_MANAGER_HPP
#define _DCD_DEVICE_MANAGER_HPP

#include <wrl.h>
#include <d2d1_2.h>
#include <d3d10_1.h>
#include <dcomp.h>
#include <dwrite.h>
#include <memory>
#include "Noncopyable.hpp"
#include "Win32Mutex.hpp"

using std::unique_ptr;
using namespace Microsoft::WRL;

namespace DCDanmaku {

    // TODO: refactor

    class DeviceManager : public Noncopyable {
    public:
        static DeviceManager* GetInstance();
    private:
        static unique_ptr<DeviceManager> sInstance;
        static Win32Mutex sMutex;
    public:
        ComPtr<ID2D1Factory2> GetD2DFactory();
        ComPtr<IDWriteFactory> GetDWriteFactory();
        ComPtr<IDCompositionDevice> GetDCompDevice();
        HRESULT CreateD2DFactory();
        HRESULT CreateDCompDevice(ComPtr<IDXGIDevice> dxgiDevice);
    private:
        ComPtr<ID2D1Factory2> mD2DFactory = nullptr;
        ComPtr<IDWriteFactory> mDWriteFactory = nullptr;
        ComPtr<IDCompositionDevice> mDCompDevice = nullptr;
    };

    using DM = DeviceManager;

}

#endif // _DCD_DEVICE_MANAGER_HPP