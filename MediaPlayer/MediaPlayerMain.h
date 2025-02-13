#pragma once
#include <DeviceResources.h>
#include <memory>
#include "StepTimer.h"
#include "VideoRender.h"
#include "AudioRender.h"
#include "PlayQueue.h"

class MediaPlayerMain
{
public:
	MediaPlayerMain(const std::shared_ptr<DeviceResources>&);
	~MediaPlayerMain();
	void CreateWindowSizeDependentResources();
	void startRenderLoop();
	void stopRenderLoop() const;

	//MediaPlayer
	void play();
	void pause();
	void selectVideo(const std::wstring& videoPath);
	void seekToTime(uint64_t timeInTicks);
	void setVolume(double volume);

	//PlayQueue
	void setPlayQueue(const std::shared_ptr<PlayQueue>& playQueue);
	void addToNextUp(const MediaFile& mediaFile);
	void clearQueue();
	void playNextMedia();
	void playPreviousMedia();
	void playCurrentMedia();

	uint64_t getVideoDuration();
	uint64_t getCurrentPosition();
	double getCurrentVolume();
	bool getIsEndOfMedia();

	void updateSizeDependentResources(uint32_t width, uint32_t height);
	void addActionOnEndMedia(std::function<void()> func);

private:
	void queueLogicUpdate();
	void update();
	bool render() const;


	std::shared_ptr<DeviceResources> m_deviceResources;
	std::unique_ptr<VideoRender> m_video;
	std::unique_ptr<AudioRender> m_audio;

	std::shared_ptr<PlayQueue> m_playQueue;
	std::vector<std::wstring> m_queue;
	bool m_isFirstMediaInQueue = true;

	concurrency::critical_section m_criticalSection;
	winrt::Windows::Foundation::IAsyncAction m_renderLoopWorker;

	StepTimer m_timer;

	std::function<void()> m_endMediaFunction;
};