#include "pch.h"
#include "VideoDecoder.h"
#include <iostream>

VideoDecoder::VideoDecoder(const std::shared_ptr<DeviceResources>& deviceResources)
    : m_deviceResources(deviceResources)
{
    winrt::check_hresult(MFStartup(MF_VERSION));
}

VideoDecoder::VideoDecoder(const std::shared_ptr<DeviceResources>& deviceResources, const std::wstring& videoPath)
    : VideoDecoder(deviceResources)
{
    loadVideo(videoPath);
}

VideoDecoder::~VideoDecoder()
{
}

void VideoDecoder::loadVideo(const std::wstring& videoPath)
{
    winrt::com_ptr<IMFAttributes> attributes;
    winrt::check_hresult(MFCreateAttributes(attributes.put(), 1));
    winrt::check_hresult(attributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE));
    winrt::check_hresult(MFCreateSourceReaderFromURL(videoPath.c_str(), attributes.get(), m_sourceReader.put()));

    winrt::com_ptr<IMFMediaType> mediaType;
    winrt::check_hresult(MFCreateMediaType(mediaType.put()));
    winrt::check_hresult(mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
    winrt::check_hresult(mediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32));
    winrt::check_hresult(m_sourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, mediaType.get()));

    winrt::com_ptr<IMFMediaType> nativeType;
    winrt::check_hresult(m_sourceReader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, nativeType.put()));
    winrt::check_hresult(MFGetAttributeSize(nativeType.get(), MF_MT_FRAME_SIZE, &m_videoWidth, &m_videoHeight));

    uint32_t frameRateNumerator = 0, frameRateDenominator = 0;
    winrt::check_hresult(MFGetAttributeRatio(nativeType.get(), MF_MT_FRAME_RATE, &frameRateNumerator, &frameRateDenominator));
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

    if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
        m_isEndOfVideo = true;
    }

    if (FAILED(hr) || !sample)
    {
        std::cerr << "Failed to read video frame or end of stream." << std::endl;
        return;
    }

    winrt::com_ptr<IMFMediaBuffer> buffer;
    winrt::check_hresult(sample->ConvertToContiguousBuffer(buffer.put()));

    BYTE* data = nullptr;
    DWORD maxLength = 0, currentLength = 0;
    winrt::check_hresult(buffer->Lock(&data, &maxLength, &currentLength));

    createBitmapFromBuffer(frame, data, m_videoWidth * 4);

    winrt::check_hresult(buffer->Unlock());
}

void VideoDecoder::createBitmapFromBuffer (VideoFrame& frame, const BYTE* data, uint32_t pitch)
{
    auto context = m_deviceResources->getD2DDeviceContext();

    D2D1_BITMAP_PROPERTIES bitmapProperties = {
        { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE },
        96.0f,
        96.0f
    };

    D2D1_SIZE_U size = { m_videoWidth, m_videoHeight };

    winrt::check_hresult(context->CreateBitmap(
        size,
        data,
        pitch,
        &bitmapProperties,
        frame.getFrameBitmap().put()
    ));
}

void VideoDecoder::seekToTime(uint64_t timeInTicks)
{
    PROPVARIANT var;
    PropVariantInit(&var);
    var.vt = VT_I8;
    var.hVal.QuadPart = timeInTicks;

    winrt::check_hresult(m_sourceReader->SetCurrentPosition(GUID_NULL, var));
}
