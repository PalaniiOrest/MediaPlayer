#include "pch.h"
#include "MediaPlayerMain.h"


using namespace winrt;
using namespace Windows::Foundation;

MediaPlayerMain::MediaPlayerMain(const std::shared_ptr<DeviceResources>& deviceResources)
	: m_deviceResources(deviceResources)
	, m_video(std::make_unique<VideoRender>(deviceResources))
	, m_audio(std::make_unique<AudioRender>(deviceResources))
	, m_playQueue(std::make_shared<PlayQueue>())
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
				std::lock_guard lock(m_criticalSection);
				queueLogicUpdate();
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

void MediaPlayerMain::queueLogicUpdate()
{
	if (m_isFirstMediaInQueue)
	{
		if (!m_playQueue->isEmpty())
		{
			m_video->loadVideo(m_playQueue->getCurrentMedia().m_filePath);
			m_audio->loadVideo(m_playQueue->getCurrentMedia().m_filePath);
			m_isFirstMediaInQueue = false;
		}
	}

	if ((m_audio->getIsEndOfMedia() || m_video->getIsEndOfMedia()) && !m_playQueue->isEmpty())
	{
		if (m_playQueue->isManualSelection())
		{
			m_playQueue->resetManualSelection();
		}
		else
		{
			m_playQueue->nextMedia();
			m_video->loadVideo(m_playQueue->getCurrentMedia().m_filePath);
			m_audio->loadVideo(m_playQueue->getCurrentMedia().m_filePath);
			m_video->play();
			m_audio->play();
			m_updateQueueUI();
		}
	}
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
	std::lock_guard lock(m_criticalSection);
	m_video->play();
	m_audio->play();
}

void MediaPlayerMain::pause()
{
	std::lock_guard lock(m_criticalSection);
	m_video->pause();
	m_audio->pause();
}

void MediaPlayerMain::selectVideo(const std::wstring& videoPath)
{
	std::lock_guard lock(m_criticalSection);
	m_video->loadVideo(videoPath);
	m_audio->loadVideo(videoPath);
}

void MediaPlayerMain::seekToTime(uint64_t timeInTicks)
{
	std::lock_guard lock(m_criticalSection);
	m_video->seekToTime(timeInTicks);
	m_audio->seekToTime(timeInTicks);
}

void MediaPlayerMain::setVolume(double volume)
{
	std::lock_guard lock(m_criticalSection);
	m_audio->setVolume(volume);
}

void MediaPlayerMain::setPlayQueue(const std::shared_ptr<PlayQueue>& playQueue)
{
	std::lock_guard lock(m_criticalSection);
	m_playQueue = playQueue;
}

void MediaPlayerMain::addToNextUp(const MediaFile& mediaFile)
{
	std::lock_guard lock(m_criticalSection);
	m_playQueue->addMediaFile(mediaFile);
	m_updateQueueUI();
}

void MediaPlayerMain::addToNextUp(const PlaylistItem& playlist)
{
	std::lock_guard lock(m_criticalSection);
	for (const auto& media : playlist.getPlaylist())
	{
		m_playQueue->addMediaFile(media);
	}
	m_updateQueueUI();
}

void MediaPlayerMain::clearQueue()
{
	std::lock_guard lock(m_criticalSection);
	m_playQueue->clear();
	m_updateQueueUI();
}

void MediaPlayerMain::playNextMedia()
{
	std::lock_guard lock(m_criticalSection);
	if( !m_playQueue->isEmpty())
	{
		m_playQueue->nextMedia();
		m_video->loadVideo(m_playQueue->getCurrentMedia().m_filePath);
		m_audio->loadVideo(m_playQueue->getCurrentMedia().m_filePath);
		m_video->play();
		m_audio->play();
		m_updateQueueUI();
	}
}

void MediaPlayerMain::playPreviousMedia()
{
	std::lock_guard lock(m_criticalSection);
	if( !m_playQueue->isEmpty())
	{
		m_playQueue->previousMedia();
		m_video->loadVideo(m_playQueue->getCurrentMedia().m_filePath);
		m_audio->loadVideo(m_playQueue->getCurrentMedia().m_filePath);
		m_video->play();
		m_audio->play();
		m_updateQueueUI();
	}
}

void MediaPlayerMain::playMediaById(const std::wstring& id)
{
	MediaFile media = m_playQueue->getMediaByIndex(id);
	//TODO: if media empty do nothing
	m_playQueue->setCurrentMedia(id);
	playCurrentMedia();
}

void MediaPlayerMain::playCurrentMedia()
{
	if (!m_playQueue->isEmpty())
	{
		auto currentMedia = m_playQueue->getCurrentMedia();
		m_video->loadVideo(currentMedia.m_filePath);
		m_audio->loadVideo(currentMedia.m_filePath);
	}
}

bool MediaPlayerMain::isQueueEmpty()
{
	std::lock_guard lock(m_criticalSection);
	return m_playQueue->isEmpty();
}

uint64_t MediaPlayerMain::getVideoDuration()
{
	std::lock_guard lock(m_criticalSection);
	return m_video->getVideoDuration();
}

uint64_t MediaPlayerMain::getCurrentPosition()
{
	std::lock_guard lock(m_criticalSection);
	return m_video->getCurrentPosition();
}

double MediaPlayerMain::getCurrentVolume()
{
	std::lock_guard lock(m_criticalSection);
	return m_audio->getCurrentVolume();
}

bool MediaPlayerMain::getIsEndOfMedia()
{
	std::lock_guard lock(m_criticalSection);
	return m_audio->getIsEndOfMedia() || m_video->getIsEndOfMedia();
}

void MediaPlayerMain::updateSizeDependentResources(uint32_t width, uint32_t height)
{
	std::lock_guard lock(m_criticalSection);
	m_deviceResources->updateSizeDependentResources(width, height);
}

void MediaPlayerMain::setUpdateQueueUIAction(std::function<void()> func)
{
	std::lock_guard lock(m_criticalSection);
	m_updateQueueUI = func;
}
