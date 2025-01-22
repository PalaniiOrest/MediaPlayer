#pragma once
#include "DeviceResources.h"

class VideoFrame
{
public:
	VideoFrame(const std::shared_ptr<DeviceResources>& deviceResources);
	VideoFrame(const std::shared_ptr<DeviceResources>& deviceResources, uint32_t width, uint32_t height);
	~VideoFrame();

	void setVideoData(std::vector<byte>& videoData);
	void createBitmapFromBuffer(winrt::com_ptr<ID2D1Bitmap>& frameBitmap);
	void render();
	void setFrameSize(uint32_t width, uint32_t height) { m_frameWidth = width, m_frameHeight = height; }

	std::vector<byte>& getBuffer() { return m_buffer; }

	operator bool() const { return !m_buffer.empty(); }

protected:
	std::shared_ptr<DeviceResources> m_deviceResources;

	winrt::com_ptr< ID2D1Bitmap> m_frameBitmap;
	std::vector<byte> m_buffer;
	uint32_t m_frameWidth;
	uint32_t m_frameHeight;

};