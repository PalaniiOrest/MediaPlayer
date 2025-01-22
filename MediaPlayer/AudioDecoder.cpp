#include "pch.h"
#include "AudioDecoder.h"
#include <iostream>

namespace
{
    constexpr uint64_t TICKS_PER_SECOND = 10'000'000;
}

AudioDecoder::AudioDecoder(const std::shared_ptr<DeviceResources>& deviceResources)
    : MediaDecoder(deviceResources)
{
    
}

void AudioDecoder::configureAudioStream()
{
    winrt::com_ptr<IMFMediaType> audioMediaType;
    winrt::check_hresult(MFCreateMediaType(audioMediaType.put()));
    winrt::check_hresult(audioMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio));
    winrt::check_hresult(audioMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM));
    winrt::check_hresult(m_sourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, audioMediaType.get()));

    m_sourceReader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE);
    m_sourceReader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);

    // set wave format
    winrt::com_ptr<IMFMediaType> outputMediaType;
    HRESULT hr = m_sourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, outputMediaType.put());
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to get current media type.");
    }

    UINT32 waveFormatSize = 0;
    WAVEFORMATEX* waveFormat = nullptr;
    hr = MFCreateWaveFormatExFromMFMediaType(outputMediaType.get(), &waveFormat, &waveFormatSize);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create wave format from media type.");
    }

    m_deviceResources->setWaveFormat(waveFormat, waveFormatSize);

    CoTaskMemFree(waveFormat);
}

void AudioDecoder::loadMedia(const std::wstring& mediaPath)
{
    MediaDecoder::loadMedia(mediaPath);
    configureAudioStream();
    m_deviceResources->updateAudioDependentResources();
    m_frameDuration = TICKS_PER_SECOND / m_deviceResources->getWaveFormat().nSamplesPerSec;
}

void AudioDecoder::decodeAudioFrame(AudioFrame& frame)
{
    DWORD flags = 0;
    winrt::com_ptr<IMFSample> sample;

    HRESULT hr = m_sourceReader->ReadSample(
        MF_SOURCE_READER_FIRST_AUDIO_STREAM,
        0,
        nullptr,
        &flags,
        nullptr,
        sample.put()
    );

    if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
        std::cout << "End of audio stream reached." << std::endl;
        m_isEndOfAudioStream = true;
        return;
    }

    if (FAILED(hr) || !sample) {
        std::cerr << "Failed to read audio frame or end of stream." << std::endl;
        return;
    }


    winrt::com_ptr<IMFMediaBuffer> mediaBuffer;
    hr = sample->ConvertToContiguousBuffer(mediaBuffer.put());
    if (FAILED(hr)) {
        std::cerr << "Failed to convert sample to contiguous buffer." << std::endl;
        return;
    }

    byte* data = nullptr;
    DWORD sampleBufferLenght = 0;

    hr = mediaBuffer->Lock(&data, nullptr, &sampleBufferLenght);
    if (FAILED(hr)) {
        std::cerr << "Failed to lock buffer." << std::endl;
        return;
    }

    std::vector<byte> audioData(sampleBufferLenght);
    memcpy_s(audioData.data(), sampleBufferLenght, data, sampleBufferLenght);
    
    frame.setAudioData(audioData);

    mediaBuffer->Unlock();
}
