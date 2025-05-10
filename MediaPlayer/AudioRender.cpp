#include "pch.h"
#include "AudioRender.h"
#include "MFAudioDecoder.h"
#include "FFMPEGAudioDecoder.h"
#include <unordered_set>

AudioRender::AudioRender(const std::shared_ptr<DeviceResources>& deviceResources)
	: m_deviceResources(deviceResources)
    , m_decoder(std::make_unique<MFAudioDecoder>(deviceResources))
    , m_frame(deviceResources)
{
}

AudioRender::~AudioRender()
{

}

void AudioRender::loadVideo(const MediaFile& videoPath)
{
    pause();
    m_decoder->loadMedia(videoPath.m_filePath);
    m_frame.updateSoundTouchResources();
}


void AudioRender::changeDecoder(DECODER decoder)
{
    m_decoder.reset();
    if (decoder == DECODER::MEDIA_FOUNDATION)
    {
        m_decoder = std::make_unique<MFAudioDecoder>(m_deviceResources);
    }
    else if (decoder == DECODER::FFMPEG)
    {
        m_decoder = std::make_unique<FFMPEGAudioDecoder>(m_deviceResources);
    }
}

void AudioRender::render()
{
    if (!m_isPlaying || m_decoder->getIsEndOfStream())
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

    m_decoder->decodeFrame(m_frame);
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
    m_decoder->seekToTime(timeInTicks);
}

void AudioRender::setVolume(double volume)
{
    m_frame.setVolume(volume);
}

void AudioRender::setTemp(float speed)
{
    m_frame.setPlaybackSpeed(speed);
}
