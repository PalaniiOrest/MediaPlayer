#include "pch.h"
#include "VideoRender.h"
#include <iostream>
#include "Constants.h"
#include "Logger.h"

VideoRender::VideoRender(const std::shared_ptr<DeviceResources>& deviceResources)
    : m_deviceResources(deviceResources)
    , m_decoder(deviceResources)
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
    m_decoder.loadMedia(videoPath);
    m_videoWidth = m_decoder.getVideoWidth();
    m_videoHeight = m_decoder.getVideoHeight();
    m_frameDuration = m_decoder.getFrameDuration();
    m_frame.setFrameSize(m_videoWidth, m_videoHeight);
}

void VideoRender::render()
{
    if (m_frame)
    {
        m_frame.render();
    }
}

//The sampling rate is traditionally measured in hertz (Hz). Examples of standard frequencies:
//
//44100 Hz is the standard frequency for audio - CD.
//48000 Hz is a frequency often used in video.
//96000 Hz or 192000 Hz is for high-quality audio.

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


    uint64_t delta = audioPlayTime - m_frameTime;

    if (m_frameTime > audioPlayTime) 
    {
        return;
    }

    m_frameTime += m_frameDuration;

    m_decoder.decodeFrame(m_frame);
}

void VideoRender::seekToTime(uint64_t timeInTicks)
{
    m_decoder.seekToTime(timeInTicks);
}

void VideoRender::setVideoEffects(std::set<VideoEffects>& effectsList)
{
    m_effectManager.setVideoEffects(effectsList);
}

uint64_t VideoRender::getVideoDuration()
{
    return m_decoder.getMediaDuration();
}

void VideoRender::updateTime()
{
    m_frameDuration = m_decoder.getFrameDuration();
}