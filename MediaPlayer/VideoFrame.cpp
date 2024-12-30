#include "pch.h"
#include "VideoFrame.h"

VideoFrame::VideoFrame(const std::shared_ptr<DeviceResources>& deviceResources)
	: m_deviceResources(deviceResources)
	, m_frameWidth(0)
	, m_frameHeight(0)
	, m_frameBitmap()
{
}

VideoFrame::VideoFrame(const std::shared_ptr<DeviceResources>& deviceResources, uint32_t width, uint32_t height)
	: m_deviceResources(deviceResources)
	, m_frameWidth(width)
	, m_frameHeight(height)
{
}

VideoFrame::~VideoFrame()
{
}

void VideoFrame::render()
{
	auto context = m_deviceResources->getD2DDeviceContext();
	context->BeginDraw();
	context->Clear(D2D1::ColorF(D2D1::ColorF::Black));
	context->DrawBitmap(m_frameBitmap.get(), D2D1::RectF(0, 0, static_cast<float>(m_frameWidth), static_cast<float>(m_frameHeight)));
	winrt::check_hresult(context->EndDraw());
	m_deviceResources->getSwapChain()->Present(1, 0);
}
