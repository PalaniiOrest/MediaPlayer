#pragma once
class MediaFile
{
public:
	MediaFile(const std::wstring& filePath);
    std::wstring calculateVideoDuration(const std::wstring& filePath);

    std::wstring m_filePath;

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
};