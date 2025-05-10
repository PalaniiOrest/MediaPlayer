#include "pch.h"
#include "MediaRecorder.h"
#include "Converter.h"

MediaRecorder::MediaRecorder()
    : m_formatContext(nullptr)
    , m_videoStreamIndex(-1)
    , m_audioStreamIndex(-1)
    , m_isRecording(false) 
{
}

MediaRecorder::~MediaRecorder()
{
}

void MediaRecorder::loadMedia(const MediaFile& media)
{
    std::string mediaPath = Converter::wstringToString(media.m_filePath);

    if (m_formatContext)
    {
        avformat_close_input(&m_formatContext);
    }

    if (avformat_open_input(&m_formatContext, mediaPath.c_str(), nullptr, nullptr) != 0) 
    {
        std::cerr << "Failed to open media file: " << mediaPath << std::endl;
        return;
    }

    if (avformat_find_stream_info(m_formatContext, nullptr) < 0) 
    {
        std::cerr << "Failed to retrieve stream info." << std::endl;
        avformat_close_input(&m_formatContext);
        return;
    }

    m_videoStreamIndex = -1;
    m_audioStreamIndex = -1;

    for (unsigned int i = 0; i < m_formatContext->nb_streams; i++) 
    {
        if (m_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && m_videoStreamIndex == -1)
        {
            m_videoStreamIndex = i;
        }
        if (m_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && m_audioStreamIndex == -1) 
        {
            m_audioStreamIndex = i;
        }
    }

    if (m_videoStreamIndex == -1) 
    {
        std::cerr << "No video stream found." << std::endl;
        return;
    }
    if (m_audioStreamIndex == -1) 
    {
        std::cerr << "No audio stream found." << std::endl;
        return;
    }

    std::cout << "Loaded media file: " << mediaPath << std::endl;
    std::cout << "Video Stream Index: " << m_videoStreamIndex << ", Audio Stream Index: " << m_audioStreamIndex << std::endl;
}

void MediaRecorder::start(double startTime, double endTime, const std::string outFile)
{
    if (!m_formatContext || m_videoStreamIndex == -1 || m_audioStreamIndex == -1) 
    {
        std::cerr << "No media loaded. Use loadMedia() first." << std::endl;
        return;
    }

    if (startTime >= endTime)
    {
        std::cerr << "Invalid time range." << std::endl;
        return;
    }

    if (m_isRecording.load()) 
    {
        std::cerr << "Recording is already in progress!" << std::endl;
        return;
    }

    m_isRecording = true;

    m_recordFuture = std::async(std::launch::async, [this, startTime, endTime, outFile]() 
        {
            record(startTime, endTime, outFile);
        });
}

void MediaRecorder::stop()
{
    if (m_isRecording.load()) 
    {
        m_isRecording = false;
        if (m_recordFuture.valid()) 
        {
            m_recordFuture.wait();
        }
    }
}

bool MediaRecorder::isRecording()
{
	return m_isRecording.load();
}

void MediaRecorder::record(double startTime, double endTime, const std::string outFile)
{
    int64_t seekTarget = startTime * AV_TIME_BASE;

    if (avformat_seek_file(m_formatContext, -1, INT64_MIN, seekTarget, INT64_MAX, AVSEEK_FLAG_BACKWARD) < 0) {
        std::cerr << "Failed to seek to startTime." << std::endl;
        m_isRecording = false;
        return;
    }


    AVFormatContext* outFormatCtx = nullptr;
    avformat_alloc_output_context2(&outFormatCtx, nullptr, nullptr, outFile.c_str());
    if (!outFormatCtx)
    {
        std::cerr << "Could not create output context." << std::endl;
        m_isRecording = false;
        return;
    }

    AVStream* videoOutStream = avformat_new_stream(outFormatCtx, nullptr);
    avcodec_parameters_copy(videoOutStream->codecpar, m_formatContext->streams[m_videoStreamIndex]->codecpar);
    videoOutStream->codecpar->codec_tag = 0;

    AVStream* audioOutStream = avformat_new_stream(outFormatCtx, nullptr);
    avcodec_parameters_copy(audioOutStream->codecpar, m_formatContext->streams[m_audioStreamIndex]->codecpar);
    audioOutStream->codecpar->codec_tag = 0;

    if (!(outFormatCtx->oformat->flags & AVFMT_NOFILE))
    {
        if (avio_open(&outFormatCtx->pb, outFile.c_str(), AVIO_FLAG_WRITE) < 0)
        {
            std::cerr << "Could not open output file." << std::endl;
            m_isRecording = false;
            return;
        }
    }

    if (avformat_write_header(outFormatCtx, nullptr) < 0)
    {
        std::cerr << "Error writing header." << std::endl;
        m_isRecording = false;
        return;
    }

    AVPacket packet = { 0 };
    double currentTime = startTime;

    double first_pts = -1;

    while (currentTime < endTime && m_isRecording.load()) 
    {

        if (av_read_frame(m_formatContext, &packet) < 0) 
        {
            break;
        }

        if (packet.stream_index != m_videoStreamIndex && packet.stream_index != m_audioStreamIndex) 
        {
            av_packet_unref(&packet);
            continue;
        }

        int64_t pts = (packet.pts != AV_NOPTS_VALUE) ? packet.pts : packet.dts;

        if (first_pts == -1 && pts != AV_NOPTS_VALUE) 
        {
            first_pts = pts;
        }

        if (first_pts != -1) 
        {
            currentTime = (pts - first_pts) * av_q2d(m_formatContext->streams[packet.stream_index]->time_base) + startTime;
        }

        if (packet.stream_index == m_videoStreamIndex) 
        {
            av_packet_rescale_ts(
                &packet,
                m_formatContext->streams[m_videoStreamIndex]->time_base,
                videoOutStream->time_base
            );
            av_interleaved_write_frame(
                outFormatCtx, 
                &packet
            );
        }
        else if (packet.stream_index == m_audioStreamIndex)
        {
            av_packet_rescale_ts(
                &packet,
                m_formatContext->streams[m_audioStreamIndex]->time_base,
                audioOutStream->time_base
            );
            av_interleaved_write_frame(
                outFormatCtx,
                &packet
            );
        }

        av_packet_unref(&packet);
    }


    av_write_trailer(outFormatCtx);
    avio_closep(&outFormatCtx->pb);
    avformat_free_context(outFormatCtx);

    m_isRecording = false;
    std::cout << "Recording complete: " << outFile << std::endl;
}