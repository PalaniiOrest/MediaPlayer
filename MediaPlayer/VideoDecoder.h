#pragma once
#include "MediaDecoder.h"
#include "VideoFrame.h"

class VideoDecoder : public MediaDecoder {
public:
    VideoDecoder(const std::shared_ptr<DeviceResources>& deviceResources);
    virtual void loadMedia(const std::wstring& mediaPath) override;
    void decodeFrame(VideoFrame& frame);
    uint32_t getVideoWidth() const { return m_videoWidth; }
    uint32_t getVideoHeight() const { return m_videoHeight; }

private:
    void configureVideoStream();

    uint32_t m_videoWidth = 0;
    uint32_t m_videoHeight = 0;
};
