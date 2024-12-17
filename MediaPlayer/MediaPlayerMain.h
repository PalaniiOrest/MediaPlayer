#pragma once
#include <DeviceResources.h>
#include <memory>
#include "StepTimer.h"
#include <ppl.h>
#include "DXRender.h"
#include "AudioRenderer.h"

class MediaPlayerMain
{
public:
	MediaPlayerMain(const std::shared_ptr<DeviceResources>&);
	~MediaPlayerMain();
	void CreateWindowSizeDependentResources();
	void StartTracking() { m_videoRenderer->StartTracking(); }
	void TrackingUpdate(float positionX) { m_pointerLocationX = positionX; }
	void StopTracking() { m_videoRenderer->StopTracking(); }
	bool IsTracking() { return m_videoRenderer->IsTracking(); }
	void startRenderLoop();
	void stopRenderLoop() const;

private:
	void update();
	bool render() const;

	std::shared_ptr<DeviceResources> m_deviceResources;
	std::unique_ptr<DXRenderer> m_videoRenderer;
	std::unique_ptr<AudioRenderer> m_audioRenderer;

	std::mutex m_mutex;
	winrt::Windows::Foundation::IAsyncAction m_renderLoopWorker;

	StepTimer m_timer;
	float m_pointerLocationX;
};

