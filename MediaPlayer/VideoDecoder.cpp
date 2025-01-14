#include "pch.h"
#include "VideoDecoder.h"
#include <iostream>


VideoDecoder::VideoDecoder(const std::shared_ptr<DeviceResources>& deviceResources)
    : MediaDecoder(deviceResources)
{
    
}

void VideoDecoder::loadMedia(const std::wstring& mediaPath)
{
    MediaDecoder::loadMedia(mediaPath);
    configureVideoStream();
}

void VideoDecoder::configureVideoStream()
{
    winrt::com_ptr<IMFMediaType> videoMediaType;
    winrt::check_hresult(MFCreateMediaType(videoMediaType.put()));
    winrt::check_hresult(videoMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
    winrt::check_hresult(videoMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32));
    winrt::check_hresult(m_sourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, videoMediaType.get()));

    winrt::com_ptr<IMFMediaType> nativeVideoType;
    winrt::check_hresult(m_sourceReader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, nativeVideoType.put()));
    winrt::check_hresult(MFGetAttributeSize(nativeVideoType.get(), MF_MT_FRAME_SIZE, &m_videoWidth, &m_videoHeight));

    uint32_t frameRateNumerator = 0, frameRateDenominator = 0;
    winrt::check_hresult(MFGetAttributeRatio(nativeVideoType.get(), MF_MT_FRAME_RATE, &frameRateNumerator, &frameRateDenominator));
    if (frameRateDenominator != 0) {
        m_frameDuration = 10'000'000 * frameRateDenominator / frameRateNumerator;
    }
}



void VideoDecoder::decodeFrame(VideoFrame& frame)
{
    DWORD flags = 0;
    winrt::com_ptr<IMFSample> sample;
    HRESULT hr = m_sourceReader->ReadSample(
        MF_SOURCE_READER_FIRST_VIDEO_STREAM,
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
        std::cerr << "Failed to read video frame or end of stream." << std::endl;
        return;
    }

    LONGLONG sampleTime = 0;
    if (SUCCEEDED(sample->GetSampleTime(&sampleTime)))
    {
        m_currentPosition = static_cast<uint64_t>(sampleTime);
    }

    winrt::com_ptr<IMFMediaBuffer> buffer;
    winrt::check_hresult(sample->ConvertToContiguousBuffer(buffer.put()));

    BYTE* data = nullptr;
    DWORD currentLength = 0;
    winrt::check_hresult(buffer->Lock(&data, nullptr, &currentLength));

    std::vector<byte> videoData(currentLength);
    memcpy_s(videoData.data(), currentLength, data, currentLength);

    frame.setVideoData(videoData);

    winrt::check_hresult(buffer->Unlock());
}