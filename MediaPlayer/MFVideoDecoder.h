#pragma once
#include "IVideoDecoder.h"

class MFVideoDecoder : public IVideoDecoder
{
public:
	MFVideoDecoder(const std::shared_ptr<DeviceResources>& deviceResources);
	virtual void loadMedia(const std::wstring& mediaPath) override;
	virtual void decodeFrame(VideoFrame& frame) override;
	virtual void seekToTime(uint64_t timeInTicks) override;

	virtual uint32_t getVideoWidth() const override;
	virtual uint32_t getVideoHeight() const override;
	virtual uint64_t getCurrentPosition() const override;
	virtual uint64_t getMediaDuration() const override;
	virtual uint64_t getFrameDuration() const override;
	virtual bool getIsEndOfStream() const override;
private:
	void initializeD3DManager();
	void calculateMediaDuration();
	void configureVideoStream();
	bool isHardwareDecoderActive();

	winrt::com_ptr<IMFSourceReader> m_sourceReader;
	std::shared_ptr<DeviceResources> m_deviceResources;
	winrt::com_ptr<IMFDXGIDeviceManager> m_dxgiDeviceManager;

	uint32_t m_videoWidth = 0;
	uint32_t m_videoHeight = 0;

	uint64_t m_currentPosition = 0;
	uint64_t m_mediaDuration = 0;
	uint64_t m_frameDuration = 0;
	bool m_isEndOfStream = false;
};

