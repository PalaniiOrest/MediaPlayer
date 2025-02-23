#pragma once
#include "IVideoDecoder.h"


class FFMPEGVideoDecoder : public IVideoDecoder {
public:
    FFMPEGVideoDecoder(const std::shared_ptr<DeviceResources>&);
    ~FFMPEGVideoDecoder();

    void loadMedia(const std::wstring& mediaPath) override;
    void decodeFrame(VideoFrame& frame) override;
    void seekToTime(uint64_t timeInTicks) override;

    uint32_t getVideoWidth() const override { return m_videoWidth; }
    uint32_t getVideoHeight() const override { return m_videoHeight; }
    uint64_t getCurrentPosition() const override { return m_currentPosition; }
    uint64_t getMediaDuration() const override { return m_mediaDuration; }
    uint64_t getFrameDuration() const override { return m_frameDuration; }
    bool getIsEndOfStream() const override { return m_isEndOfStream; }

private:
    void initializeHWDecoder();

    std::shared_ptr<DeviceResources> m_deviceResources;
    D3D11MultithreadLock m_multithreadLock;

    AVFormatContext* m_formatCtx = nullptr;
    AVCodecContext* m_codecCtx = nullptr;
    const AVCodec* m_codec = nullptr;
    AVStream* m_videoStream = nullptr;
    AVBufferRef* m_hwDeviceCtx = nullptr;
    SwsContext* m_swsCtx = nullptr;
    AVFrame* m_hwFrame = nullptr;
    AVFrame* m_swFrame = nullptr;
    AVFrame* m_bgraFrame = nullptr;
    int m_videoStreamIndex = -1;

    uint32_t m_videoWidth = 0;
    uint32_t m_videoHeight = 0;
    uint64_t m_currentPosition = 0;
    uint64_t m_mediaDuration = 0;
    uint64_t m_frameDuration = 0;
    bool m_isEndOfStream = false;
};
