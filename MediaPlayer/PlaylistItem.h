#pragma once
#include "MediaFile.h"
#include "PlayQueue.h"

class PlaylistItem
{
public:
	PlaylistItem();
	PlaylistItem(const std::wstring& name);

	void addFile(const MediaFile& mediaFile);
	MediaFile getFileByID(const std::wstring& mediaId);
	std::vector<MediaFile> getPlaylist() const;
	std::wstring getName() { return m_name; }
	void setName(const std::wstring& name) { m_name = name; }

private:
	std::wstring m_name;
	std::vector<MediaFile> m_fileList;

};