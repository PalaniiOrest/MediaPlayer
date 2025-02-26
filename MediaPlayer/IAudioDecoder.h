#pragma once
#include "AudioFrame.h"

class IAudioDecoder
{
public:
    virtual ~IAudioDecoder() = default;

    virtual void loadMedia(const std::wstring& mediaPath) = 0;
    virtual void decodeFrame(AudioFrame& frame) = 0;
    virtual void skipFrame(uint32_t numFrames = 1) = 0;
    virtual void seekToTime(uint64_t timeInTicks) = 0;

    virtual bool getIsEndOfStream() = 0;
};

