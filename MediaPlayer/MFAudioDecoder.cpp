#include "pch.h"
#include "MFAudioDecoder.h"
#include "Constants.h"

MFAudioDecoder::MFAudioDecoder(const std::shared_ptr<DeviceResources>& deviceResources)
    : m_deviceResources(deviceResources)
{

}

MFAudioDecoder::~MFAudioDecoder()
{
}

void MFAudioDecoder::loadMedia(const MediaFile& mediaPath)
{
    winrt::com_ptr<IMFAttributes> attributes;
    winrt::check_hresult(MFCreateAttributes(attributes.put(), 1));
    winrt::check_hresult(attributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE));
    winrt::check_hresult(MFCreateSourceReaderFromURL(mediaPath.m_filePath.c_str(), attributes.get(), m_sourceReader.put()));

    calculateMediaDuration();
    configureAudioStream();

    m_deviceResources->updateAudioDependentResources();
    m_frameDuration = TICKS_PER_SECOND / m_deviceResources->getWaveFormat().nSamplesPerSec;
    m_isEndOfStream = false;
}

void MFAudioDecoder::decodeFrame(AudioFrame& frame)
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

    if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
    {
        m_isEndOfStream = true;
        return;
    }

    if (FAILED(hr) || !sample)
    {
        return;
    }

    winrt::com_ptr<IMFMediaBuffer> mediaBuffer;
    hr = sample->ConvertToContiguousBuffer(mediaBuffer.put());
    if (FAILED(hr))
    {
        return;
    }

    byte* data = nullptr;
    DWORD sampleBufferLenght = 0;

    hr = mediaBuffer->Lock(&data, nullptr, &sampleBufferLenght);
    if (FAILED(hr))
    {
        return;
    }

    std::vector<byte> audioData(sampleBufferLenght);
    memcpy_s(audioData.data(), sampleBufferLenght, data, sampleBufferLenght);

    frame.setAudioData(audioData);

    mediaBuffer->Unlock();
}

void MFAudioDecoder::skipFrame(uint32_t numFrames)
{

    DWORD flags = 0;
    winrt::com_ptr<IMFSample> sample;

    uint32_t skipped = 0;
    while (skipped < numFrames)
    {

        HRESULT hr = m_sourceReader->ReadSample(
            MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            0,
            nullptr,
            &flags,
            nullptr,
            sample.put()
        );
        skipped++;
    }
}

void MFAudioDecoder::seekToTime(uint64_t timeInTicks)
{
    PROPVARIANT var;
    PropVariantInit(&var);
    var.vt = VT_I8;
    var.hVal.QuadPart = timeInTicks;

    winrt::check_hresult(m_sourceReader->SetCurrentPosition(GUID_NULL, var));
    m_isEndOfStream = false;
}

bool MFAudioDecoder::getIsEndOfStream()
{
    return m_isEndOfStream;
}

void MFAudioDecoder::configureAudioStream()
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

void MFAudioDecoder::calculateMediaDuration()
{
    PROPVARIANT varDuration;
    PropVariantInit(&varDuration);

    winrt::com_ptr<IMFPresentationDescriptor> presentationDescriptor;
    winrt::com_ptr<IMFMediaSource> mediaSource;

    winrt::check_hresult(m_sourceReader->GetServiceForStream(
        MF_SOURCE_READER_MEDIASOURCE,
        GUID_NULL,
        IID_PPV_ARGS(mediaSource.put())
    ));

    winrt::check_hresult(mediaSource->CreatePresentationDescriptor(presentationDescriptor.put()));
    winrt::check_hresult(presentationDescriptor->GetUINT64(MF_PD_DURATION, reinterpret_cast<UINT64*>(&m_mediaDuration)));

    PropVariantClear(&varDuration);
}
