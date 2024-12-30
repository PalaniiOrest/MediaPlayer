#pragma once
#include "DeviceResources.h"

class VideoFrame
{
public:
	VideoFrame(const std::shared_ptr<DeviceResources>& deviceResources);
	VideoFrame(const std::shared_ptr<DeviceResources>& deviceResources, uint32_t width, uint32_t height);
	~VideoFrame();

	void render();

	void setFrameSize(uint32_t width, uint32_t height) { m_frameWidth = width, m_frameHeight = height; }
	winrt::com_ptr<ID2D1Bitmap>& getFrameBitmap() { return m_frameBitmap; }
	
	operator bool() const { return m_frameBitmap ? true : false; }
protected:
	std::shared_ptr<DeviceResources> m_deviceResources;
	uint32_t m_frameWidth;
	uint32_t m_frameHeight;
	winrt::com_ptr<ID2D1Bitmap> m_frameBitmap;
};

