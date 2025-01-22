#include "pch.h"
#include "VideoFrame.h"

VideoFrame::VideoFrame(const std::shared_ptr<DeviceResources>& deviceResources)
	: m_deviceResources(deviceResources)
	, m_frameWidth(0)
	, m_frameHeight(0)
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

void VideoFrame::setVideoData(std::vector<byte>& videoData)
{
	m_buffer = videoData;
}

void VideoFrame::createBitmapFromBuffer(winrt::com_ptr<ID2D1Bitmap>& frameBitmap)
{
	auto context = m_deviceResources->getD2DDeviceContext();

	D2D1_BITMAP_PROPERTIES bitmapProperties = {
		{ DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE },
		96.0f,
		96.0f
	};

	D2D1_SIZE_U size = { m_frameWidth, m_frameHeight };

	winrt::check_hresult(context->CreateBitmap(
		size,
		m_buffer.data(),
		size.width * 4,
		&bitmapProperties,
		frameBitmap.put()
	));
}

void VideoFrame::render()
{
	winrt::com_ptr<ID2D1Bitmap> frameBitmap;

	createBitmapFromBuffer(frameBitmap);

	auto context = m_deviceResources->getD2DDeviceContext();
	context->BeginDraw();
	context->Clear(D2D1::ColorF(D2D1::ColorF::Black));
	context->DrawBitmap(
		frameBitmap.get(),
		D2D1::RectF(
			0,
			0,
			static_cast<float>(m_deviceResources->getVideoWidth()),
			static_cast<float>(m_deviceResources->getVideoHeight())));
	winrt::check_hresult(context->EndDraw());
	m_deviceResources->getSwapChain()->Present(1, 0);
}