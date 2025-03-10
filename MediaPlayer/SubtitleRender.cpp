#include "pch.h"
#include "SubtitleRender.h"
#include <SubtitleDecoder.h>
#include "Constants.h"

SubtitleRender::SubtitleRender(const std::shared_ptr<DeviceResources>& deviceResources)
    : m_deviceResources(deviceResources)
    , m_decoder(std::make_unique<SubtitleDecoder>())
{
}

void SubtitleRender::update(const StepTimer& timer)
{
    if (m_frameList.empty())
    {
        return;
    }
    XAUDIO2_VOICE_STATE state;
    m_deviceResources->getSourceVoice()->GetState(&state);

    float playbackSpeed = m_deviceResources->getPlaybackSpeed();
    uint64_t adjustedSamplesPlayed = static_cast<uint64_t>(state.SamplesPlayed * playbackSpeed);
    uint64_t audioPlayTime = adjustedSamplesPlayed * TICKS_PER_SECOND / m_deviceResources->getWaveFormat().nSamplesPerSec;

    for (const auto& frame : m_frameList)
    {
        if ((audioPlayTime >= frame.getStartTime() * TICKS_PER_SECOND) &&
            (audioPlayTime <= frame.getEndTime() * TICKS_PER_SECOND))
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_subtitleText = frame.getText();
            break;
        }
        else
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_subtitleText = L"";
        }
    }
}

void SubtitleRender::render()
{
    if (m_updateTextCallback)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_updateTextCallback(m_subtitleText);
    }
}

void SubtitleRender::loadMedia(const MediaFile& media)
{
    m_decoder->loadMedia(media);
    m_frameList = m_decoder->getDecodedSubtitles();
}

void SubtitleRender::loadSubtitles(const std::wstring& path)
{
    m_frameList = m_decoder->getDecodedSubtitlesFromeSrt("C:\\Users\\palan\\Downloads\\coca.srt");
}

void SubtitleRender::setUpdateTextCallback(std::function<void(std::wstring)> callback)
{
    m_updateTextCallback = std::move(callback);
}
