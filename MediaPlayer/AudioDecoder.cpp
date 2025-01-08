#include "pch.h"
#include "AudioDecoder.h"
#include <iostream>

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
}

void AudioDecoder::loadMedia(const std::wstring& mediaPath)
{
    MediaDecoder::loadMedia(mediaPath);
    configureAudioStream();
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
