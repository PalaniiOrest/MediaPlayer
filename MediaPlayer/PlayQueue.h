#pragma once
#include "MediaFile.h"
#include <vector>

class PlayQueue
{
public:
    PlayQueue();
    PlayQueue(const MediaFile&);

    void clear();
    void addMediaFile(const MediaFile& mediaFile);
    MediaFile getMediaFileByID(int id);
    void removeMediaFileByID(int id);
    void moveUp(int id);
    std::vector<MediaFile> getQueue() const;

    MediaFile getCurrentMedia();
    void setCurrentMedia(const std::wstring& mediaId);
    MediaFile getMediaByIndex(const std::wstring& mediaId);
    size_t getCurrentIndex() const;
    bool isManualSelection() const { return m_manualSelection; }
    void resetManualSelection() { m_manualSelection = false; }

    void nextMedia();
    void previousMedia();
    bool isEmpty();
private:
    std::vector<MediaFile> m_queue;
    size_t m_currentIndex = 0;
    bool m_manualSelection = false;
};

