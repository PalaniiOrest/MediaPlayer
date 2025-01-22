#pragma once
#include "MediaDecoder.h"
#include "AudioFrame.h"

class AudioDecoder : public MediaDecoder {
public:
    AudioDecoder(const std::shared_ptr<DeviceResources>& deviceResources);

    virtual void loadMedia(const std::wstring& mediaPath) override;
    void decodeAudioFrame(AudioFrame& frame);
    bool getIsEndOfAudioStream() { return m_isEndOfAudioStream; }
    void setIsEndOfAudioStream(bool isEndOfAudioStream) { m_isEndOfAudioStream = isEndOfAudioStream; }

private:
    void configureAudioStream();

    bool m_isEndOfAudioStream = false;
};
