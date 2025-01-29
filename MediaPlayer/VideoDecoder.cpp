#include "pch.h"
#include "VideoDecoder.h"
#include <iostream>
#include "Constants.h"



VideoDecoder::VideoDecoder(const std::shared_ptr<DeviceResources>& deviceResources)
    : MediaDecoder(deviceResources)
{
    initializeD3DManager();
}

void VideoDecoder::loadMedia(const std::wstring& mediaPath)
{
    winrt::com_ptr<IMFAttributes> sourceReaderAttributes;
    winrt::check_hresult(MFCreateAttributes(sourceReaderAttributes.put(), 3));
    winrt::check_hresult(sourceReaderAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING, TRUE));
    winrt::check_hresult(sourceReaderAttributes->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, TRUE));
    winrt::check_hresult(sourceReaderAttributes->SetUnknown(MF_SOURCE_READER_D3D_MANAGER, m_dxgiDeviceManager.get()));

    winrt::check_hresult( MFCreateSourceReaderFromURL(mediaPath.c_str(), sourceReaderAttributes.get(), m_sourceReader.put()));
    configureVideoStream();

    calculateMediaDuration();
}

void VideoDecoder::initializeD3DManager()
{
    UINT resetToken = 0;
    winrt::check_hresult(MFCreateDXGIDeviceManager(&resetToken, m_dxgiDeviceManager.put()));

    winrt::com_ptr<ID3D11Device> d3dDevice = m_deviceResources->getD3DDevice();
    winrt::check_hresult(m_dxgiDeviceManager->ResetDevice(d3dDevice.get(), resetToken));
}

void VideoDecoder::configureVideoStream()
{
    winrt::com_ptr<IMFMediaType> videoMediaType;
    winrt::check_hresult(MFCreateMediaType(videoMediaType.put()));
    winrt::check_hresult(videoMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
    winrt::check_hresult(videoMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_ARGB32));

    winrt::check_hresult(m_sourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, videoMediaType.get()));

    winrt::com_ptr<IMFMediaType> nativeVideoType;
    winrt::check_hresult(m_sourceReader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, nativeVideoType.put()));
    winrt::check_hresult(MFGetAttributeSize(nativeVideoType.get(), MF_MT_FRAME_SIZE, &m_videoWidth, &m_videoHeight));

    winrt::check_hresult(m_sourceReader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE));
    winrt::check_hresult(m_sourceReader->SetStreamSelection(MF_SOURCE_READER_FIRST_VIDEO_STREAM, TRUE));

    uint32_t frameRateNumerator = 0, frameRateDenominator = 0;
    winrt::check_hresult(MFGetAttributeRatio(nativeVideoType.get(), MF_MT_FRAME_RATE, &frameRateNumerator, &frameRateDenominator));
    if (frameRateDenominator != 0) {
        m_frameDuration = TICKS_PER_SECOND * frameRateDenominator / frameRateNumerator;
    }
}

void VideoDecoder::decodeFrame(VideoFrame& frame)
{
    DWORD flags = 0;
    winrt::com_ptr<IMFSample> sample;
    winrt::check_hresult(m_sourceReader->ReadSample(
        MF_SOURCE_READER_FIRST_VIDEO_STREAM,
        0,
        nullptr,
        &flags,
        nullptr,
        sample.put()
    ));

    if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
    {
        m_isEndOfStream = true;
        return;
    }

    if (!sample)
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
    winrt::check_hresult(sample->GetBufferByIndex(0, buffer.put()));

    winrt::com_ptr<IMFDXGIBuffer> dxgiBuffer;
    HRESULT hr = (buffer->QueryInterface(__uuidof(IMFDXGIBuffer), reinterpret_cast<void**>(dxgiBuffer.put())));

    winrt::com_ptr<ID3D11Texture2D> texture;
    winrt::check_hresult(dxgiBuffer->GetResource(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(texture.put())));

    frame.setVideoData(texture);
}

bool VideoDecoder::isHardwareDecoderActive()
{
    if (!m_sourceReader) return false;

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

    if (FAILED(hr) || !sample)
    {
        std::cerr << "Cant get sample." << std::endl;
        return false;
    }

    winrt::com_ptr<IMFMediaBuffer> buffer;
    hr = sample->GetBufferByIndex(0, buffer.put());
    if (FAILED(hr))
    {
        std::cerr << "Cant get buffer." << std::endl;
        return false;
    }

    winrt::com_ptr<IMFDXGIBuffer> dxgiBuffer;
    hr = buffer->QueryInterface(__uuidof(IMFDXGIBuffer), reinterpret_cast<void**>(dxgiBuffer.put()));
    if (FAILED(hr))
    {
        std::cerr << "Buffer isnt dxgi." << std::endl;
        return false;
    }

    winrt::com_ptr<ID3D11Texture2D> texture;
    hr = dxgiBuffer->GetResource(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(texture.put()));
    if (FAILED(hr))
    {
        std::cerr << "cant get texture." << std::endl;
        return false;
    }

    std::cout << "its hardware decoding." << std::endl;
    return true;
}