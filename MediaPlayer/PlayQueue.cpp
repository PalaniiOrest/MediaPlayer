
#include "pch.h"
#include "PlayQueue.h"

PlayQueue::PlayQueue() {}

PlayQueue::PlayQueue(const MediaFile& mediaFile)
{
    m_queue.push_back(mediaFile);
}

void PlayQueue::clear()
{
    m_queue.clear();
    m_currentIndex = 0;
}

void PlayQueue::addMediaFile(const MediaFile& mediaFile)
{
    m_queue.push_back(mediaFile);
}

MediaFile PlayQueue::getMediaFileByID(int id)
{
    if (id >= 0 && id < static_cast<int>(m_queue.size()))
    {
        return m_queue[id];
    }
    return MediaFile(L"");
}

void PlayQueue::removeMediaFileByID(int id)
{
    if (id >= 0 && id < static_cast<int>(m_queue.size()))
    {
        m_queue.erase(m_queue.begin() + id);
        if (m_currentIndex >= m_queue.size())
        {
            m_currentIndex = 0;
        }
    }
}

void PlayQueue::moveUp(int id)
{
    if (id > 0 && id < static_cast<int>(m_queue.size()))
    {
        std::swap(m_queue[id], m_queue[id - 1]);
    }
}

std::vector<MediaFile> PlayQueue::getQueue() const
{
    return m_queue;
}

MediaFile PlayQueue::getCurrentMedia()
{
    if (!m_queue.empty())
    {
        return m_queue[m_currentIndex];
    }
    return MediaFile(L"");
}

void PlayQueue::setCurrentMedia(const std::wstring& mediaId)
{
    for (size_t i = 0; i < m_queue.size(); ++i)
    {
        if (m_queue[i].m_id == mediaId)
        {
            m_currentIndex = i;
            return;
        }
    }
}

MediaFile PlayQueue::getMediaByIndex(const std::wstring& mediaId)
{
    for (size_t i = 0; i < m_queue.size(); ++i)
    {
        if (m_queue[i].m_id == mediaId)
        {
            return m_queue[i];
        }
    }
    return MediaFile(L"");
}

size_t PlayQueue::getCurrentIndex() const
{
    return m_currentIndex;
}


void PlayQueue::nextMedia()
{
    if (!m_queue.empty())
    {
        m_currentIndex = (m_currentIndex + 1) % m_queue.size();
    }
}

void PlayQueue::previousMedia()
{
    if (!m_queue.empty())
    {
        m_currentIndex = (m_currentIndex - 1) % m_queue.size();
    }
}

bool PlayQueue::isEmpty()
{
    return m_queue.empty();
}