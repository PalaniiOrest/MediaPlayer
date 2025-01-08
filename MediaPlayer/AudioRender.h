#pragma once
#include "DeviceResources.h"
#include "StepTimer.h"
#include "AudioDecoder.h"
#include "AudioFrame.h"

class AudioRender
{
public:
	AudioRender(const std::shared_ptr<DeviceResources>& deviceResources);
	~AudioRender();

    void loadVideo(const std::wstring& videoPath);
    void render();
    void update(const StepTimer&);

    void play();
    void pause();
    void seekToTime(uint64_t timeInTicks);

private:
	std::shared_ptr<DeviceResources> m_deviceResources;
    AudioDecoder m_decoder;
    AudioFrame m_frame;

    uint64_t m_frameDuration = 0;
    uint64_t m_lastFrameTime = 0;


    bool m_isPlaying = false;
    bool isVideoEnd = false;
};