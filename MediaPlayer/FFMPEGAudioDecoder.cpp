#include "pch.h"
#include "FFmpegAudioDecoder.h"


FFMPEGAudioDecoder::FFMPEGAudioDecoder(const std::shared_ptr<DeviceResources>& deviceResources)
    : m_deviceResources(deviceResources)
{
    avformat_network_init();
}

FFMPEGAudioDecoder::~FFMPEGAudioDecoder()
{
    cleanup();
}

void FFMPEGAudioDecoder::loadMedia(const MediaFile& mediaPath)
{
    cleanup();
    std::string path(mediaPath.m_filePath.begin(), mediaPath.m_filePath.end());

    if (avformat_open_input(&m_formatContext, path.c_str(), nullptr, nullptr) != 0)
        throw std::runtime_error("Failed to open media file");

    if (avformat_find_stream_info(m_formatContext, nullptr) < 0)
        throw std::runtime_error("Failed to retrieve stream info");

    openCodecContext();

    WAVEFORMATEX waveFormat = {};
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = m_codecContext->ch_layout.nb_channels;
    waveFormat.nSamplesPerSec = m_codecContext->sample_rate; 
    waveFormat.wBitsPerSample = 16;
    waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8; 
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign; 
    waveFormat.cbSize = 0;

    m_deviceResources->setWaveFormat(&waveFormat, sizeof(WAVEFORMATEX));

    m_packet = av_packet_alloc();
    m_frame = av_frame_alloc();
    m_isEndOfStream = false;
}

void FFMPEGAudioDecoder::openCodecContext()
{
    for (unsigned i = 0; i < m_formatContext->nb_streams; i++)
    {
        if (m_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            m_audioStreamIndex = i;
            break;
        }
    }

    if (m_audioStreamIndex == -1)
    {
        throw std::runtime_error("No audio stream found");
    }

    const AVCodec* codec = avcodec_find_decoder(m_formatContext->streams[m_audioStreamIndex]->codecpar->codec_id);
    if (!codec)
    {
        throw std::runtime_error("Failed to find decoder");
    }

    m_codecContext = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(m_codecContext, m_formatContext->streams[m_audioStreamIndex]->codecpar);

    if (avcodec_open2(m_codecContext, codec, nullptr) < 0)
    {
        throw std::runtime_error("Failed to open codec");
    }

    m_swrContext = swr_alloc();
    if (!m_swrContext) {
        throw std::runtime_error("swr_alloc failed");
    }

    if (m_codecContext->ch_layout.order == AV_CHANNEL_ORDER_UNSPEC) 
    {
        av_channel_layout_default(&m_codecContext->ch_layout, m_codecContext->ch_layout.nb_channels);
    }

    AVChannelLayout in_ch_layout = m_codecContext->ch_layout;
    AVChannelLayout out_ch_layout;
    av_channel_layout_default(&out_ch_layout, 2);

    int ret = swr_alloc_set_opts2(&m_swrContext,
        &out_ch_layout, AV_SAMPLE_FMT_S16, m_codecContext->sample_rate,
        &in_ch_layout, m_codecContext->sample_fmt, m_codecContext->sample_rate,
        0, nullptr);

    if (ret < 0)
    {
        throw std::runtime_error("swr_alloc_set_opts2 failed");
    }

    if (swr_init(m_swrContext) < 0)
    {
        throw std::runtime_error("swr_init failed");
    }
}


void FFMPEGAudioDecoder::decodeFrame(AudioFrame& frame)
{
    while (true)
    {
        if (!m_packet || av_read_frame(m_formatContext, m_packet) < 0)
        {
            m_isEndOfStream = true;
            return;
        }

        if (m_packet->stream_index == m_audioStreamIndex)
        {
            break;
        }

        av_packet_unref(m_packet);
    }

    if (m_packet->stream_index == m_audioStreamIndex)
    {
        if (avcodec_send_packet(m_codecContext, m_packet) == 0)
        {
            while (avcodec_receive_frame(m_codecContext, m_frame) == 0)
            {
                int outSamples = av_rescale_rnd(
                    swr_get_delay(m_swrContext, m_codecContext->sample_rate) + m_frame->nb_samples,
                    m_codecContext->sample_rate,
                    m_codecContext->sample_rate,
                    AV_ROUND_UP
                );

                int outBufferSize = av_samples_get_buffer_size(
                    nullptr,
                    2,
                    outSamples,
                    AV_SAMPLE_FMT_S16,
                    1
                );

                std::vector<uint8_t> audioData(outBufferSize);
                uint8_t* outBuffer = audioData.data();

                int convertedSamples = swr_convert(
                    m_swrContext,
                    &outBuffer, outSamples,
                    (const uint8_t**)m_frame->data,
                    m_frame->nb_samples
                );

                if (convertedSamples < 0)
                {
                    std::cerr << "Error while converting audio samples." << std::endl;
                    return;
                }

                frame.setAudioData(audioData);
            }
        }
    }
}

void FFMPEGAudioDecoder::skipFrame(uint32_t numFrames)
{
    for (uint32_t i = 0; i < numFrames; i++)
    {
        if (av_read_frame(m_formatContext, m_packet) < 0)
        {
            m_isEndOfStream = true;
            return;
        }
        av_packet_unref(m_packet);
    }
}

void FFMPEGAudioDecoder::seekToTime(uint64_t timeInTicks)
{
    int64_t timestamp = av_rescale_q(timeInTicks, AV_TIME_BASE_Q,
        m_formatContext->streams[m_audioStreamIndex]->time_base);
    av_seek_frame(m_formatContext, m_audioStreamIndex, timestamp, AVSEEK_FLAG_BACKWARD);
    m_isEndOfStream = false;
}

bool FFMPEGAudioDecoder::getIsEndOfStream()
{
    return m_isEndOfStream;
}

void FFMPEGAudioDecoder::cleanup()
{
    if (m_packet) av_packet_free(&m_packet);
    if (m_frame) av_frame_free(&m_frame);
    if (m_codecContext) avcodec_free_context(&m_codecContext);
    if (m_swrContext) swr_free(&m_swrContext);
    if (m_formatContext) avformat_close_input(&m_formatContext);
}
