#pragma once
#include "DeviceResources.h"

class MediaDecoder {
public:
    MediaDecoder(const std::shared_ptr<DeviceResources>& deviceResources);
    virtual ~MediaDecoder();

    virtual void loadMedia(const std::wstring& mediaPath);
    void seekToTime(uint64_t timeInTicks);
    uint64_t getMediaDuration() const { return m_mediaDuration; }
    uint64_t getFrameDuration() { return m_frameDuration; }


protected:
    void calculateMediaDuration();  

    std::shared_ptr<DeviceResources> m_deviceResources;
    winrt::com_ptr<IMFSourceReader> m_sourceReader;

    uint64_t m_mediaDuration = 0;
    uint64_t m_frameDuration = 0;
    uint64_t m_lastFrameTime = 0;
    bool m_isEndOfStream = false;
};