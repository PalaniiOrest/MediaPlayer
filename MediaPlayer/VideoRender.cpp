#include "pch.h"
#include "VideoRender.h"
#include <iostream>

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
    m_decoder.loadVideo(videoPath);
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

void VideoRender::update(const StepTimer& timer)
{
    if (!m_isPlaying)
    {
        return;
    }

    uint64_t currentTime = timer.GetTotalTicks();

    if (currentTime - m_lastFrameTime < m_frameDuration)
    {
        return;
    }
    m_lastFrameTime = currentTime;

    m_decoder.decodeFrame(m_frame);
}

void VideoRender::seekToTime(uint64_t timeInTicks)
{
    m_decoder.seekToTime(timeInTicks);
}

void VideoRender::updateTime()
{
    m_frameDuration = m_decoder.getFrameDuration();
}
