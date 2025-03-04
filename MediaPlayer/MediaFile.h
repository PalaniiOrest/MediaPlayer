#pragma once
class MediaFile
{
public:
	MediaFile(const std::wstring& filePath);

    std::wstring m_filePath;
    std::wstring m_fileExtension;

    std::wstring m_id;
    std::wstring m_name;
    std::wstring m_creatorName;
    std::wstring m_albumTitle;
    std::wstring m_genre;
    std::wstring m_year;
    std::wstring m_description;
    std::wstring m_copyright;
    std::wstring m_language;
    std::wstring m_publisher;
    std::wstring m_duration;
    std::wstring m_videoCodec;
    std::wstring m_audioCodec;
    std::wstring m_resolution;
    std::wstring m_frameRate;
    std::wstring m_bitRate;

private:
    std::wstring getMetadata(AVDictionary* metadata, const char* key);
    std::wstring getVideoDuration(AVFormatContext* formatCtx);
};