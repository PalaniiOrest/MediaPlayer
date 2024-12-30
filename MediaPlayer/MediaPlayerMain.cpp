#include "pch.h"
#include "MediaPlayerMain.h"


using namespace winrt;
using namespace Windows::Foundation;

MediaPlayerMain::MediaPlayerMain(const std::shared_ptr<DeviceResources>& deviceResources)
	: m_deviceResources(deviceResources)
	, m_video(std::make_unique<VideoRender>(deviceResources))
	, m_audioRenderer(std::make_unique<AudioRenderer>())
	, m_pointerLocationX(0.0f)
{
}

MediaPlayerMain::~MediaPlayerMain()
{
	
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
	});
}

bool MediaPlayerMain::render() const
{
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}

	m_video->render();

	return true;
}

void MediaPlayerMain::play()
{
	m_video->play();
}

void MediaPlayerMain::pause()
{
	m_video->pause();
}

void MediaPlayerMain::selectVideo(const std::wstring& videoPath)
{
	m_video->loadVideo(videoPath);
}
