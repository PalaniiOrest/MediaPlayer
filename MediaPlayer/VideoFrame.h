#pragma once
#include "DeviceResources.h"
#include "D3D11MultithreadLock.h"

class VideoFrame
{
public:
	VideoFrame(const std::shared_ptr<DeviceResources>& deviceResources);
	VideoFrame(const std::shared_ptr<DeviceResources>& deviceResources, uint32_t width, uint32_t height);
	~VideoFrame();

	void setVideoData(const winrt::com_ptr<ID3D11Texture2D>& texture);
	void createBitmapFromTexure();
	void render();
	void setFrameSize(uint32_t width, uint32_t height) { m_frameWidth = width, m_frameHeight = height; }

	std::vector<byte>& getBuffer() { return m_buffer; }

	operator bool() const { return m_texture != nullptr; }

protected:
	std::shared_ptr<DeviceResources> m_deviceResources;

	winrt::com_ptr<ID3D11Texture2D> m_texture;
	winrt::com_ptr<ID2D1Bitmap> m_frameBitmap;
	std::vector<byte> m_buffer;
	uint32_t m_frameWidth;
	uint32_t m_frameHeight;

	D3D11MultithreadLock m_multithreadLock;
};