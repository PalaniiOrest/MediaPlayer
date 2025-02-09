#pragma once
#include "MediaFile.h"

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
	std::list<MediaFile> getQueue();

	MediaFile getFirstMedia();
	void deleteFirstMedia();

	bool isEmpty();
private:
	std::list<MediaFile> m_queue;

};

