#include "pch.h"
#include "VideoFrame.h"
#include <wincodec.h>
#include <fstream>

VideoFrame::VideoFrame(const std::shared_ptr<DeviceResources>& deviceResources)
	: m_deviceResources(deviceResources)
	, m_frameWidth(0)
	, m_frameHeight(0)
	, m_multithreadLock(deviceResources->getD3DMultithread())
{

}

VideoFrame::VideoFrame(const std::shared_ptr<DeviceResources>& deviceResources, uint32_t width, uint32_t height)
	: m_deviceResources(deviceResources)
	, m_frameWidth(width)
	, m_frameHeight(height)
	, m_multithreadLock(deviceResources->getD3DMultithread())
{
}

VideoFrame::~VideoFrame()
{
}

void VideoFrame::setVideoData(const winrt::com_ptr<ID3D11Texture2D>& texture)
{
	m_texture = texture;
	
}

void VideoFrame::createBitmapFromTexure()
{
	std::lock_guard<D3D11MultithreadLock> lock(m_multithreadLock);

	D3D11_TEXTURE2D_DESC textureDesc;
	m_texture->GetDesc(&textureDesc);
	if (textureDesc.Format != DXGI_FORMAT_B8G8R8A8_UNORM)
	{
		return;
	}

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
	std::lock_guard<D3D11MultithreadLock> lock(m_multithreadLock);

	createBitmapFromTexure();
	auto context = m_deviceResources->getD2DDeviceContext();
	context->BeginDraw();
	context->Clear(D2D1::ColorF(D2D1::ColorF::Black));
	context->DrawBitmap(
		m_frameBitmap.get(),
		D2D1::RectF(
			0,
			0,
			static_cast<float>(m_deviceResources->getVideoWidth()),
			static_cast<float>(m_deviceResources->getVideoHeight())
		)
	);
	winrt::check_hresult(context->EndDraw());
	m_deviceResources->getSwapChain()->Present(0, 0);
}


void VideoFrame::saveScreenshot(const std::wstring& filePath, const GUID& format)
{
	std::lock_guard<D3D11MultithreadLock> lock(m_multithreadLock);

	if (!m_texture)
	{
		return;
	}

	D3D11_TEXTURE2D_DESC textureDesc;
	m_texture->GetDesc(&textureDesc);

	D3D11_TEXTURE2D_DESC copyDesc = textureDesc;
	copyDesc.Usage = D3D11_USAGE_STAGING;
	copyDesc.BindFlags = 0;
	copyDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	copyDesc.MiscFlags = 0;

	winrt::com_ptr<ID3D11Texture2D> stagingTexture;
	winrt::check_hresult(m_deviceResources->getD3DDevice()->CreateTexture2D(&copyDesc, nullptr, stagingTexture.put()));

	m_deviceResources->getD3DDeviceContext()->CopyResource(stagingTexture.get(), m_texture.get());

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	winrt::check_hresult(m_deviceResources->getD3DDeviceContext()->Map(stagingTexture.get(), 0, D3D11_MAP_READ, 0, &mappedResource));

	winrt::com_ptr<IWICImagingFactory> wicFactory;
	winrt::check_hresult(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(wicFactory.put())));

	winrt::com_ptr<IWICBitmapEncoder> encoder;
	winrt::check_hresult(wicFactory->CreateEncoder(format, nullptr, encoder.put()));

	winrt::com_ptr<IWICStream> stream;
	winrt::check_hresult(wicFactory->CreateStream(stream.put()));
	winrt::check_hresult(stream->InitializeFromFilename(filePath.c_str(), GENERIC_WRITE));

	winrt::check_hresult(encoder->Initialize(stream.get(), WICBitmapEncoderNoCache));

	winrt::com_ptr<IWICBitmapFrameEncode> frame;
	winrt::com_ptr<IPropertyBag2> propertyBag;
	winrt::check_hresult(encoder->CreateNewFrame(frame.put(), propertyBag.put()));
	winrt::check_hresult(frame->Initialize(propertyBag.get()));
	winrt::check_hresult(frame->SetSize(textureDesc.Width, textureDesc.Height));

	WICPixelFormatGUID pixelFormat = GUID_WICPixelFormat32bppBGRA;
	winrt::check_hresult(frame->SetPixelFormat(&pixelFormat));

	winrt::check_hresult(frame->WritePixels(
		textureDesc.Height,
		mappedResource.RowPitch,
		mappedResource.RowPitch * textureDesc.Height,
		static_cast<BYTE*>(mappedResource.pData)
	));

	winrt::check_hresult(frame->Commit());
	winrt::check_hresult(encoder->Commit());

	m_deviceResources->getD3DDeviceContext()->Unmap(stagingTexture.get(), 0);
}
