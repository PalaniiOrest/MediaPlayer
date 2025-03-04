#include "pch.h"
#include "MediaFile.h"
#include <filesystem>
#include <random>
#include "Converter.h"

MediaFile::MediaFile(const std::wstring& filePath)
    : m_filePath(filePath),
    m_id(std::to_wstring(std::hash<std::wstring>{}(filePath) ^ (std::random_device{}() << 1))) {

    std::filesystem::path path(filePath);
    m_name = path.filename().wstring();
    m_fileExtension = path.extension().wstring();
    std::transform(m_fileExtension.begin(), m_fileExtension.end(), m_fileExtension.begin(), ::towlower);

    std::string utf8Path = Converter::wstringToString(filePath);

    avformat_network_init();
    AVFormatContext* formatCtx = avformat_alloc_context();

    if (avformat_open_input(&formatCtx, utf8Path.c_str(), nullptr, nullptr) == 0) {
        if (avformat_find_stream_info(formatCtx, nullptr) >= 0) {
            m_duration = getVideoDuration(formatCtx);
            m_bitRate = std::to_wstring(formatCtx->bit_rate / 1000) + L" kbps";

            for (unsigned int i = 0; i < formatCtx->nb_streams; i++) {
                AVStream* stream = formatCtx->streams[i];
                AVCodecParameters* codecParams = stream->codecpar;
                const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);

                if (codecParams->codec_type == AVMEDIA_TYPE_VIDEO) {
                    m_videoCodec = Converter::stringToWString(codec->long_name);
                    m_resolution = std::to_wstring(codecParams->width) + L"x" + std::to_wstring(codecParams->height);

                    if (stream->avg_frame_rate.num != 0 && stream->avg_frame_rate.den != 0) {
                        double fps = av_q2d(stream->avg_frame_rate);
                        std::wstringstream ss;
                        ss << std::fixed << std::setprecision(2) << fps;
                        m_frameRate = ss.str() + L" fps";
                    }
                }
                else if (codecParams->codec_type == AVMEDIA_TYPE_AUDIO) {
                    m_audioCodec = Converter::stringToWString(codec->long_name);
                }
            }

            m_creatorName = getMetadata(formatCtx->metadata, "artist");
            m_albumTitle = getMetadata(formatCtx->metadata, "album");
            m_genre = getMetadata(formatCtx->metadata, "genre");
            m_year = getMetadata(formatCtx->metadata, "date");
            m_description = getMetadata(formatCtx->metadata, "comment");
            m_copyright = getMetadata(formatCtx->metadata, "copyright");
            m_language = getMetadata(formatCtx->metadata, "language");
            m_publisher = getMetadata(formatCtx->metadata, "publisher");
        }
        avformat_close_input(&formatCtx);
    }
    avformat_free_context(formatCtx);
}


std::wstring MediaFile::getMetadata(AVDictionary* metadata, const char* key) {
    AVDictionaryEntry* tag = av_dict_get(metadata, key, nullptr, 0);
    return tag ? Converter::stringToWString(tag->value) : L"";
}

std::wstring MediaFile::getVideoDuration(AVFormatContext* formatCtx) {
    if (formatCtx->duration != AV_NOPTS_VALUE) {
        int64_t duration = formatCtx->duration / AV_TIME_BASE;
        int hours = duration / 3600;
        int minutes = (duration % 3600) / 60;
        int seconds = duration % 60;

        std::wstringstream ss;
        ss << std::setw(2) << std::setfill(L'0') << hours << L":"
            << std::setw(2) << std::setfill(L'0') << minutes << L":"
            << std::setw(2) << std::setfill(L'0') << seconds;
        return ss.str();
    }
    return L"00:00:00";
}
