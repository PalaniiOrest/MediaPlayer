#include "pch.h"
#include "PlaylistItem.h"

PlaylistItem::PlaylistItem()
{
}

PlaylistItem::PlaylistItem(const std::wstring& name)
    : m_name(name)
{
}

void PlaylistItem::addFile(const MediaFile& mediaFile)
{
    m_fileList.push_back(mediaFile);
}

MediaFile PlaylistItem::getFileByID(const std::wstring& id)
{
    for (size_t i = 0; i < m_fileList.size(); ++i)
    {
        if (m_fileList[i].m_id == id)
        {
            return m_fileList[i];
        }
    }
    return MediaFile(L"");
}

std::vector<MediaFile> PlaylistItem::getPlaylist() const
{
    return m_fileList;
}
