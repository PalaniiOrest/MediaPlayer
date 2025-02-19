#pragma once
#include "MediaDecoder.h"
#include "VideoFrame.h"

class VideoDecoder : public MediaDecoder {
public:
    VideoDecoder(const std::shared_ptr<DeviceResources>& deviceResources);
    virtual void loadMedia(const std::wstring& mediaPath) override;
    void initializeD3DManager();
    void decodeFrame(VideoFrame& frame);
    uint32_t getVideoWidth() const { return m_videoWidth; }
    uint32_t getVideoHeight() const { return m_videoHeight; }
    uint64_t getCurrentPosition() { return m_currentPosition; }
    bool getIsEndOfStream() { return m_isEndOfStream; }
private:
    void configureVideoStream();
    bool isHardwareDecoderActive();

    std::shared_ptr<DeviceResources> m_deviceResources;

    uint32_t m_videoWidth = 0;
    uint32_t m_videoHeight = 0;

    uint64_t m_currentPosition = 0;

    winrt::com_ptr<IMFDXGIDeviceManager> m_dxgiDeviceManager;
};
