#include "pch.h"
#include "DXRender.h"


DXRenderer::DXRenderer(const std::shared_ptr<DeviceResources>& deviceResources)
    : m_deviceResources(deviceResources)
{
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

void DXRenderer::Render()
{
    if (!m_loadingComplete)
    {
        return;
    }

    if (m_startPlaying)
    {

    }
}

void DXRenderer::CreateDeviceDependentResources()
{
    
    m_startPlaying = true;
}

void DXRenderer::CreateWindowSizeDependentResources()
{
}

void DXRenderer::StartTracking()
{
}

void DXRenderer::StopTracking()
{
}

bool DXRenderer::IsTracking()
{
    return true;    
}
