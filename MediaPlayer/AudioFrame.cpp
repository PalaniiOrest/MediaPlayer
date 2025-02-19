#include "pch.h"
#include "AudioFrame.h"

namespace
{
    static constexpr int MAX_BUFFERS_COUNT = 3;
}

AudioFrame::AudioFrame(const std::shared_ptr<DeviceResources>& deviceResources)
	: m_deviceResources(deviceResources)
{
    m_buffers.resize(MAX_BUFFERS_COUNT);
}

AudioFrame::~AudioFrame()
{
}

void AudioFrame::setAudioData(const std::vector<BYTE>& audioData)
{
    if (audioData.empty()) 
    {
        throw std::invalid_argument("Audio data cannot be empty.");
    }

    m_buffers[m_currentStreamBuffer] = audioData;
}

void AudioFrame::play()
{
    XAUDIO2_VOICE_STATE state;
    for (;;)
    {
        m_deviceResources->getSourceVoice()->GetState(&state);
        if (state.BuffersQueued < MAX_BUFFERS_COUNT - 1)
            break;

        WaitForSingleObject(m_deviceResources->getVoiceContext().hBufferEndEvent, INFINITE);
    }

    XAUDIO2_BUFFER buf = {};
    buf.AudioBytes = m_buffers[m_currentStreamBuffer].size();
    buf.pAudioData = m_buffers[m_currentStreamBuffer].data();
    m_deviceResources->getSourceVoice()->SubmitSourceBuffer(&buf);

    m_currentStreamBuffer++;
    m_currentStreamBuffer %= MAX_BUFFERS_COUNT;
}


void AudioFrame::stop()
{
    if (m_deviceResources->getSourceVoice()) {
        m_deviceResources->getSourceVoice()->Stop(0);
        m_deviceResources->getSourceVoice()->FlushSourceBuffers();
    }
}

bool AudioFrame::isPlaying()
{
    XAUDIO2_VOICE_STATE state = {};
    if (m_deviceResources->getSourceVoice()) 
    {
        m_deviceResources->getSourceVoice()->GetState(&state);
    }
    return state.BuffersQueued > 0;
}

void AudioFrame::setVolume(double volume)
{
    if (m_deviceResources->getSourceVoice())
    {
        HRESULT hr = m_deviceResources->getSourceVoice()->SetVolume(volume);
        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to set volume.");
        }
        else
        {
            m_volume = volume;
        }
    }
}