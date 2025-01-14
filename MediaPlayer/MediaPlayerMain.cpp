#include "pch.h"
#include "MediaPlayerMain.h"


using namespace winrt;
using namespace Windows::Foundation;

MediaPlayerMain::MediaPlayerMain(const std::shared_ptr<DeviceResources>& deviceResources)
	: m_deviceResources(deviceResources)
	, m_video(std::make_unique<VideoRender>(deviceResources))
	, m_audio(std::make_unique<AudioRender>(deviceResources))
	, m_pointerLocationX(0.0f)
{
	winrt::check_hresult(MFStartup(MF_VERSION));
}

MediaPlayerMain::~MediaPlayerMain()
{
	winrt::check_hresult(MFShutdown());
}

void MediaPlayerMain::CreateWindowSizeDependentResources()
{
}

void MediaPlayerMain::startRenderLoop()
{
	if (m_renderLoopWorker != nullptr && m_renderLoopWorker.Status() == AsyncStatus::Started)
	{
		return;
	}

	auto workItemHandler = winrt::Windows::System::Threading::WorkItemHandler([this](const winrt::Windows::Foundation::IAsyncAction& action)
		{
			while (action.Status() == winrt::Windows::Foundation::AsyncStatus::Started)
			{
				std::lock_guard lock(m_mutex);

				update();
				render();

			}
		});

	m_renderLoopWorker = winrt::Windows::System::Threading::ThreadPool::RunAsync(
		workItemHandler,
		winrt::Windows::System::Threading::WorkItemPriority::High,
		winrt::Windows::System::Threading::WorkItemOptions::TimeSliced
	);
}

void MediaPlayerMain::stopRenderLoop() const
{
	m_renderLoopWorker.Cancel();
}

void MediaPlayerMain::update()
{
	m_timer.Tick([&]()
		{
			m_video->update(m_timer);
			m_audio->update(m_timer);
		});
}

bool MediaPlayerMain::render() const
{
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}

	m_video->render();
	m_audio->render();

	return true;
}

void MediaPlayerMain::play()
{
	m_video->play();
	m_audio->play();
}

void MediaPlayerMain::pause()
{
	m_video->pause();
	m_audio->pause();
}

void MediaPlayerMain::selectVideo(const std::wstring& videoPath)
{
	m_video->loadVideo(videoPath);
	m_audio->loadVideo(videoPath);
}

void MediaPlayerMain::seekToTime(uint64_t timeInTicks)
{
	m_video->seekToTime(timeInTicks);
	m_audio->seekToTime(timeInTicks);
}

void MediaPlayerMain::setVideoEffects(std::set<VideoEffects>& effectsList)
{
	m_video->setVideoEffects(effectsList);
}

void MediaPlayerMain::setAudioEffects(std::set<AudioEffects>& effectsList)
{
	m_audio->setAudioEffects(effectsList);
}

uint64_t MediaPlayerMain::getVideoDuration()
{
	return m_video->getVideoDuration();
}

uint64_t MediaPlayerMain::getCurrentPosition()
{
	return m_video->getCurrentPosition();
}
