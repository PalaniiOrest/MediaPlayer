#pragma once
#include "DeviceResources.h"
#include "IAudioDecoder.h"

class FFmpegAudioDecoder : public IAudioDecoder
{
public:
    FFmpegAudioDecoder(const std::shared_ptr<DeviceResources>&);
    ~FFmpegAudioDecoder();

    void loadMedia(const std::wstring& mediaPath) override;
    void decodeFrame(AudioFrame& frame) override;
    void skipFrame(uint32_t numFrames = 1) override;
    void seekToTime(uint64_t timeInTicks) override;
    bool getIsEndOfStream() override;

private:
    void cleanup();
    void openCodecContext();

    std::shared_ptr<DeviceResources> m_deviceResources;

    AVFormatContext* m_formatContext = nullptr;
    AVCodecContext* m_codecContext = nullptr;
    SwrContext* m_swrContext = nullptr;
    AVPacket* m_packet = nullptr;
    AVFrame* m_frame = nullptr;

    int m_audioStreamIndex = -1;
    bool m_isEndOfStream = false;
};
