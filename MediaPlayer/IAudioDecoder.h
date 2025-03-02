#pragma once
#include "AudioFrame.h"
#include "MediaFile.h"

class IAudioDecoder
{
public:
    virtual ~IAudioDecoder() = default;

    virtual void loadMedia(const MediaFile& media) = 0;
    virtual void decodeFrame(AudioFrame& frame) = 0;
    virtual void skipFrame(uint32_t numFrames = 1) = 0;
    virtual void seekToTime(uint64_t timeInTicks) = 0;

    virtual bool getIsEndOfStream() = 0;
};

