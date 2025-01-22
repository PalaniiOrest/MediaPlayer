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
	void setVolume(double volume);

	void setVideoEffects(std::set<VideoEffects>& effectsList);
	void setAudioEffects(std::set<AudioEffects>& effectsList);

	uint64_t getVideoDuration();
	uint64_t getCurrentPosition();
	double getCurrentVolume();

	void updateSizeDependentResources(uint32_t width, uint32_t height);

private:
	void update();
	bool render() const;


	std::shared_ptr<DeviceResources> m_deviceResources;
	std::unique_ptr<VideoRender> m_video;
	std::unique_ptr<AudioRender> m_audio;

	concurrency::critical_section m_criticalSection;
	winrt::Windows::Foundation::IAsyncAction m_renderLoopWorker;

	StepTimer m_timer;
	float m_pointerLocationX;
};