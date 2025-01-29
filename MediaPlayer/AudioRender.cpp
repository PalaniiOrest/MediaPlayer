#include "pch.h"
#include "AudioRender.h"

AudioRender::AudioRender(const std::shared_ptr<DeviceResources>& deviceResources)
	: m_deviceResources(deviceResources)
    , m_decoder(deviceResources)
    , m_frame(deviceResources)
{
}

AudioRender::~AudioRender()
{

}

void AudioRender::loadVideo(const std::wstring& videoPath)
{
    pause();
    m_decoder.loadMedia(videoPath);
    m_frameDuration = m_decoder.getFrameDuration();
}

void AudioRender::render()
{
    if (!m_isPlaying || m_decoder.getIsEndOfAudioStream())
    {
        return;
    }

    if (m_frame)
    {
        m_frame.play();
    }
}

void AudioRender::update(const StepTimer& timer)
{
    if (m_isPlaying == false)
    {
        return;
    }

    m_decoder.decodeAudioFrame(m_frame);
}

void AudioRender::play()
{
    m_isPlaying = true;
    m_deviceResources->getSourceVoice()->Start(0, 0);
}

void AudioRender::pause()
{
    m_isPlaying = false;
    m_frame.stop();
}

void AudioRender::seekToTime(uint64_t timeInTicks)
{
    m_decoder.seekToTime(timeInTicks);
    m_decoder.setIsEndOfAudioStream(false);
}

void AudioRender::setVolume(double volume)
{
    m_frame.setVolume(volume);
}