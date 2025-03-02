#include "pch.h"
#include "VideoRender.h"
#include <iostream>
#include "Logger.h"
#include "MFVideoDecoder.h"
#include "FFMPEGVideoDecoder.h"

VideoRender::VideoRender(const std::shared_ptr<DeviceResources>& deviceResources)
    : m_deviceResources(deviceResources)
    , m_decoder(std::make_unique<MFVideoDecoder>(deviceResources))
    , m_frame(deviceResources)
{
    winrt::check_hresult(MFStartup(MF_VERSION));
}

VideoRender::~VideoRender()
{
    MFShutdown();
}

void VideoRender::loadVideo(const std::wstring& videoPath)
{
    pause();
    m_decoder->loadMedia(videoPath);
    m_videoWidth = m_decoder->getVideoWidth();
    m_videoHeight = m_decoder->getVideoHeight();
    m_frameDuration = m_decoder->getFrameDuration();
    m_frame.setFrameSize(m_videoWidth, m_videoHeight);
    m_lastFrameTime = 0;
    m_frameTime = 0;
}

void VideoRender::changeDecoder(DECODER decoder)
{
    m_decoder.reset();
    if (decoder == DECODER::MEDIA_FOUNDATION)
    {
        m_decoder = std::make_unique<MFVideoDecoder>(m_deviceResources);
    }
    else if (decoder == DECODER::FFMPEG)
    {
        m_decoder = std::make_unique<FFMPEGVideoDecoder>(m_deviceResources);
    }
}

void VideoRender::saveCurrentFrameAsScreenshot(const std::wstring& path, const GUID& format)
{
    m_frame.saveScreenshot(path, format);
}

void VideoRender::render()
{
    if (m_frame)
    {
        m_frame.render();
    }
}

void VideoRender::update(const StepTimer& timer)
{
    if (!m_isPlaying)
    {
        return;
    }

    uint64_t currentTime = timer.GetTotalTicks();

    XAUDIO2_VOICE_STATE state;
    m_deviceResources->getSourceVoice()->GetState(&state);

    uint64_t audioPlayTime = state.SamplesPlayed * TICKS_PER_SECOND / m_deviceResources->getWaveFormat().nSamplesPerSec;

    if (audioPlayTime <= m_frameTime)
    {
        return;
    }

    while (audioPlayTime > m_frameTime)
    {
        m_decoder->decodeFrame(m_frame);
        m_frameTime += m_frameDuration;
    }
}

void VideoRender::seekToTime(uint64_t timeInTicks)
{
    m_decoder->seekToTime(timeInTicks);
}

uint64_t VideoRender::getVideoDuration()
{
    return m_decoder->getMediaDuration();
}

void VideoRender::updateTime()
{
    m_frameDuration = m_decoder->getFrameDuration();
}