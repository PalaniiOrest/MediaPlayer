#pragma once
#include <SubtitleFrame.h>
#include "MediaFile.h"

class ISubtitleDecoder
{
public:
    virtual ~ISubtitleDecoder() = 0;

    virtual void loadMedia(const MediaFile& media) = 0;
    virtual std::vector<SubtitleFrame> getDecodedSubtitles() = 0;
    virtual std::vector<SubtitleFrame> getDecodedSubtitlesFromeSrt(const std::string& filePath) = 0;

};

