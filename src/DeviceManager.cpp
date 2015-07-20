#include <mutex>
#include "DeviceManager.hpp"

namespace DCDanmaku {

    unique_ptr<DeviceManager> DeviceManager::sInstance;
    Win32Mutex DeviceManager::sMutex;

    DeviceManager* DeviceManager::GetInstance() {
        if (sInstance == nullptr) {
            std::lock_guard<Win32Mutex> locker(sMutex);
            if (sInstance == nullptr) {
                sInstance = std::make_unique<DeviceManager>();
            }
        }
        return sInstance.get();
    }

    ComPtr<ID2D1Factory2> DeviceManager::GetD2DFactory() {
        return mD2DFactory;
    }

    HRESULT DeviceManager::CreateD2DFactory() {
        const D2D1_FACTORY_OPTIONS options = { D2D1_DEBUG_LEVEL_INFORMATION };
        return D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, mD2DFactory.GetAddressOf());
    }

    ComPtr<IDWriteFactory> DeviceManager::GetDWriteFactory() {
        if (mDWriteFactory == nullptr) {
            DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &mDWriteFactory);
        }
        return mDWriteFactory;
    }

    ComPtr<IDCompositionDevice> DeviceManager::GetDCompDevice() {
        return mDCompDevice;
    }

    HRESULT DeviceManager::CreateDCompDevice(ComPtr<IDXGIDevice> dxgiDevice) {
        return DCompositionCreateDevice(dxgiDevice.Get(), IID_PPV_ARGS(&mDCompDevice));
    }

}