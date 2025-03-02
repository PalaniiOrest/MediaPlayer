#pragma once
#include "IAudioDecoder.h"
#include "DeviceResources.h"

class MFAudioDecoder : public IAudioDecoder
{
public:
	MFAudioDecoder(const std::shared_ptr<DeviceResources>&);
	~MFAudioDecoder();

    virtual void loadMedia(const MediaFile& mediaPath) override;
    virtual void decodeFrame(AudioFrame& frame) override;
    virtual void skipFrame(uint32_t numFrames = 1) override;
    virtual void seekToTime(uint64_t timeInTicks) override;

    virtual bool getIsEndOfStream() override;

private:
    void configureAudioStream();
    void calculateMediaDuration();

    std::shared_ptr<DeviceResources> m_deviceResources;
    winrt::com_ptr<IMFSourceReader> m_sourceReader;

    uint64_t m_mediaDuration = 0;
    uint64_t m_frameDuration = 0;
    bool m_isEndOfStream = false;
};

