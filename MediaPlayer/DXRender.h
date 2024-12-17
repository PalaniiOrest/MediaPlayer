#pragma once
#include "DeviceResources.h"

class DXRenderer
{
public:
    DXRenderer(const std::shared_ptr<DeviceResources>& deviceResources);
    void Render();
    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();
    void StartTracking();
    void StopTracking();
    bool IsTracking();

private:
    std::shared_ptr<DeviceResources> m_deviceResources{ nullptr };

    bool m_startPlaying = false;
    bool m_loadingComplete;
    bool m_tracking;
};

