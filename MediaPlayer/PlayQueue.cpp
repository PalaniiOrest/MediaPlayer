#include "pch.h"
#include "PlayQueue.h"

PlayQueue::PlayQueue()
{
}

PlayQueue::PlayQueue(const MediaFile& mediaFile)
{
    m_queue.push_back(mediaFile);
}

void PlayQueue::clear()
{
    m_queue.clear();
}

void PlayQueue::addMediaFile(const MediaFile& mediaFile)
{
    m_queue.push_back(mediaFile);
}

MediaFile PlayQueue::getMediaFileByID(int id)
{
    return MediaFile(L"");
}

void PlayQueue::removeMediaFileByID(int id)
{
}

void PlayQueue::moveUp(int id)
{
}

std::list<MediaFile> PlayQueue::getQueue()
{
    return m_queue;
}

MediaFile PlayQueue::getFirstMedia()
{
    if (!m_queue.empty())
    {
        return m_queue.front();
    }
}

void PlayQueue::deleteFirstMedia()
{
    if (!m_queue.empty())
    {
        m_queue.pop_front();
    }
}

bool PlayQueue::isEmpty()
{
    return m_queue.empty();
}
