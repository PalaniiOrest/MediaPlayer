#include "pch.h"
#include "SoundTouchProcessor.h"

SoundTouchProcessor::SoundTouchProcessor(const std::shared_ptr<DeviceResources>& deviceResources)
    : m_deviceResources(deviceResources)
{
    m_soundTouch.setSampleRate(44100);
    m_soundTouch.setChannels(2);
    m_soundTouch.setTempo(1.0f);
}


void SoundTouchProcessor::setPlaybackSpeed(float speed)
{
    m_deviceResources->setPlaybackSpeed(speed);
}

void SoundTouchProcessor::processAudio(std::vector<byte>& buffer)
{
    if (buffer.empty())
    {
        return;
    }

    WAVEFORMATEX waveFormat = m_deviceResources->getWaveFormat();
    const size_t bytesPerSample = sizeof(int16_t);
    const size_t numChannels = waveFormat.nChannels;
    const size_t samplesCount = buffer.size() / (bytesPerSample * numChannels);

    std::vector<int16_t> inputSamples(samplesCount * numChannels);
    memcpy(inputSamples.data(), buffer.data(), buffer.size());

    std::vector<float> floatSamples(samplesCount * numChannels);
    constexpr float scale = 1.0f / 32768.0f;
    for (size_t i = 0; i < inputSamples.size(); i++)
    {
        floatSamples[i] = inputSamples[i] * scale;
    }

    m_soundTouch.setTempo(m_deviceResources->getPlaybackSpeed());
    m_soundTouch.putSamples(floatSamples.data(), samplesCount);

    std::vector<float> outputFloatSamples(samplesCount * numChannels);
    size_t receivedSamples = m_soundTouch.receiveSamples(outputFloatSamples.data(), samplesCount);

    std::vector<int16_t> outputSamples(receivedSamples * numChannels);
    constexpr float maxInt16 = 32767.0f;
    for (size_t i = 0; i < outputSamples.size(); i++)
    {
        outputSamples[i] = static_cast<int16_t>(std::clamp(outputFloatSamples[i] * maxInt16, -32768.0f, maxInt16));
    }

    buffer.resize(outputSamples.size() * bytesPerSample);
    memcpy(buffer.data(), outputSamples.data(), buffer.size());
}




void SoundTouchProcessor::updateSoundTouchResources()
{
    WAVEFORMATEX waveFormat = m_deviceResources->getWaveFormat();

    m_soundTouch.setSampleRate(waveFormat.nSamplesPerSec);
    m_soundTouch.setChannels(waveFormat.nChannels);

    m_deviceResources->setPlaybackSpeed(0.5f);
    m_soundTouch.setTempo(m_deviceResources->getPlaybackSpeed());
}