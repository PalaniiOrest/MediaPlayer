#pragma once
#include "VideoFrame.h"
#include "MediaFile.h"

class IVideoDecoder
{
public:
    virtual ~IVideoDecoder() = default;

    virtual void loadMedia(const MediaFile& media) = 0;
    virtual void decodeFrame(VideoFrame& frame) = 0;
    virtual void skipFrame(uint32_t numFrames = 1) = 0;
    virtual void seekToTime(uint64_t timeInTicks) = 0;

    virtual uint32_t getVideoWidth() const = 0;
    virtual uint32_t getVideoHeight() const = 0;
    virtual uint64_t getCurrentPosition() const = 0;
    virtual uint64_t getMediaDuration() const = 0;
    virtual uint64_t getFrameDuration() const = 0;
    virtual bool getIsEndOfStream() const = 0;

protected:

};

