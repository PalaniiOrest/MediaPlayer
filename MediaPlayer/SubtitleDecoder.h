#pragma once
#include "SubtitleFrame.h"
#include "ISubtitleDecoder.h"

class SubtitleDecoder : public ISubtitleDecoder
{
public:
    SubtitleDecoder();
    ~SubtitleDecoder() override;

    void loadMedia(const MediaFile& media) override;
    std::vector<SubtitleFrame> getDecodedSubtitles() override;
    std::vector<SubtitleFrame> getDecodedSubtitlesFromeSrt(const std::string& filePath) override;

private:
    AVFormatContext* m_formatCtx = nullptr;
    AVCodecContext* m_codecCtx = nullptr;
    int m_subtitleStreamIndex = -1;
};