#pragma once
#include <DeviceResources.h>
#include <memory>
#include "StepTimer.h"
#include <ppl.h>
#include "VideoRender.h"
#include "AudioRender.h"

class MediaPlayerMain
{
public:
	MediaPlayerMain(const std::shared_ptr<DeviceResources>&);
	~MediaPlayerMain();
	void CreateWindowSizeDependentResources();
	void TrackingUpdate(float positionX) { m_pointerLocationX = positionX; }
	void startRenderLoop();
	void stopRenderLoop() const;

	void play();
	void pause();
	void selectVideo(const std::wstring& videoPath);
	void seekToTime(uint64_t timeInTicks);

	uint64_t getVideoDuration();

private:
	void update();
	bool render() const;


	std::shared_ptr<DeviceResources> m_deviceResources;
	std::unique_ptr<VideoRender> m_video;
	std::unique_ptr<AudioRender> m_audio;

	std::mutex m_mutex;
	winrt::Windows::Foundation::IAsyncAction m_renderLoopWorker;

	StepTimer m_timer;
	float m_pointerLocationX;
};