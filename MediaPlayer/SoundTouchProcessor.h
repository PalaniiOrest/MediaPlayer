#pragma once
#include "SoundTouch.h"
#include "DeviceResources.h"

class SoundTouchProcessor
{
public:
    SoundTouchProcessor(const std::shared_ptr<DeviceResources>&);
    void setPlaybackSpeed(float speed);
    void updateSoundTouchResources();

protected:
    std::shared_ptr<DeviceResources> m_deviceResources;
    soundtouch::SoundTouch m_soundTouch;

    void processAudio(std::vector<byte>& buffer);
};

