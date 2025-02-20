#pragma once
#include "VideoFrame.h"

class IVideoDecoder
{
public:
    virtual ~IVideoDecoder() = default;

    virtual void loadMedia(const std::wstring& mediaPath) = 0;
    virtual void decodeFrame(VideoFrame& frame) = 0;
    virtual void seekToTime(uint64_t timeInTicks) = 0;

    virtual uint32_t getVideoWidth() const = 0;
    virtual uint32_t getVideoHeight() const = 0;
    virtual uint64_t getCurrentPosition() const = 0;
    virtual uint64_t getMediaDuration() const = 0;
    virtual uint64_t getFrameDuration() const = 0;
    virtual bool getIsEndOfStream() const = 0;

protected:

};

