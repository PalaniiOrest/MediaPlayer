#pragma once
#include "DeviceResources.h"
#include "StepTimer.h"
#include "VideoDecoder.h"
#include "VideoFrame.h"

class VideoRender
{
public:
    VideoRender(const std::shared_ptr<DeviceResources>& deviceResources);
    ~VideoRender();

    void loadVideo(const std::wstring& videoPath);
    void render();
    void update(const StepTimer&);

    void play() { m_isPlaying = true; }
    void pause() { m_isPlaying = false; }
    void seekToTime(uint64_t timeInTicks);


private:
    void updateTime();

    std::shared_ptr<DeviceResources> m_deviceResources;
    VideoDecoder m_decoder;
    VideoFrame m_frame;

    uint32_t m_videoWidth = 0;
    uint32_t m_videoHeight = 0;
    uint64_t m_frameDuration = 0;
    uint64_t m_lastFrameTime = 0;


    bool m_isPlaying = false;
    bool isVideoEnd = false;
};

