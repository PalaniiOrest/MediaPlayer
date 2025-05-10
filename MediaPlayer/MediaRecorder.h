#pragma once
#include "MediaFile.h"

class MediaRecorder
{
public:
	MediaRecorder();
	~MediaRecorder();
	
	void loadMedia(const MediaFile& media);
	void start(double startTime, double endTime, const std::string outFile);
	void stop();
	bool isRecording();

private:
	AVFormatContext* m_formatContext;
	int m_videoStreamIndex;
	int m_audioStreamIndex;
	std::atomic<bool> m_isRecording;
	std::future<void> m_recordFuture;

	void record(double startTime, double endTime, const std::string outFile);
};

