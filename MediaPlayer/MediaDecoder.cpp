#include "pch.h"
#include "MediaDecoder.h"

MediaDecoder::MediaDecoder()
{
}

MediaDecoder::~MediaDecoder() = default;

void MediaDecoder::loadMedia(const std::wstring& mediaPath)
{
    winrt::com_ptr<IMFAttributes> attributes;
    winrt::check_hresult(MFCreateAttributes(attributes.put(), 1));
    winrt::check_hresult(attributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE));
    winrt::check_hresult(MFCreateSourceReaderFromURL(mediaPath.c_str(), attributes.get(), m_sourceReader.put()));

    calculateMediaDuration();
}

void MediaDecoder::seekToTime(uint64_t timeInTicks)
{
    PROPVARIANT var;
    PropVariantInit(&var);
    var.vt = VT_I8;
    var.hVal.QuadPart = timeInTicks;

    winrt::check_hresult(m_sourceReader->SetCurrentPosition(GUID_NULL, var));
    m_isEndOfStream = false;
}


void MediaDecoder::calculateMediaDuration()
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
