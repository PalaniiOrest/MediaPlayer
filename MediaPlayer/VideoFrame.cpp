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

void VideoFrame::setVideoData(const winrt::com_ptr<ID3D11Texture2D>& texture)
{
	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);

	if (desc.Format != DXGI_FORMAT_B8G8R8A8_UNORM)
	{
		return;
	}
	m_texture = texture;

}

void VideoFrame::createBitmapFromTexure()
{
	winrt::com_ptr<IDXGISurface> dxgiSurface;
	winrt::check_hresult(m_texture->QueryInterface(dxgiSurface.put()));

	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96.0f,
			96.0f
		);

	winrt::com_ptr<ID2D1Bitmap1> d2dBitmap;
	winrt::check_hresult(m_deviceResources->getD2DDeviceContext()->CreateBitmapFromDxgiSurface(
		dxgiSurface.get(),
		&bitmapProperties,
		d2dBitmap.put()
	));

	m_frameBitmap = d2dBitmap;
}

void VideoFrame::render()
{
	createBitmapFromTexure();
	auto context = m_deviceResources->getD2DDeviceContext();
	context->BeginDraw();
	context->Clear(D2D1::ColorF(D2D1::ColorF::Black));
	context->DrawBitmap(
		m_frameBitmap.get(),
		D2D1::RectF(
			0,
			0,
			static_cast<float>(m_frameWidth),
			static_cast<float>(m_frameHeight)
		)
	);
	winrt::check_hresult(context->EndDraw());
	m_deviceResources->getSwapChain()->Present(0, 0);

}