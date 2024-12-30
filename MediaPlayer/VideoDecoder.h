#pragma once
#include "DeviceResources.h"
#include "VideoFrame.h"
#include <string>

class VideoDecoder
{
public:
	VideoDecoder(const std::shared_ptr<DeviceResources>& deviceResources);
	VideoDecoder(const std::shared_ptr<DeviceResources>& deviceResources, const std::wstring& videoPath);
	~VideoDecoder();

	void loadVideo(const std::wstring& videoPath);
	void decodeFrame(VideoFrame& frame);
	void createBitmapFromBuffer(VideoFrame& frame, const BYTE* data, uint32_t pitch);
	void seekToTime(uint64_t timeInTicks);
	uint32_t getVideoWidth() { return m_videoWidth; }
	uint32_t getVideoHeight() { return m_videoHeight; }
	uint64_t getFrameDuration() { return m_frameDuration; }
	uint64_t getLastFrameTime() { return m_lastFrameTime; }

protected:
	std::shared_ptr<DeviceResources> m_deviceResources;
	winrt::com_ptr<IMFSourceReader> m_sourceReader;

	uint32_t m_videoWidth = 0;
	uint32_t m_videoHeight = 0;
	uint64_t m_frameDuration = 0;
	uint64_t m_lastFrameTime = 0;

	bool m_isEndOfVideo = false;
};

