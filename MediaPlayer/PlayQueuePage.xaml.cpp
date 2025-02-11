#include "pch.h"
#include "PlayQueuePage.xaml.h"
#if __has_include("PlayQueuePage.g.cpp")
#include "PlayQueuePage.g.cpp"
#endif
#include <MediaFileViewModel.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MediaPlayer::implementation
{
    int32_t PlayQueuePage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void PlayQueuePage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
	void PlayQueuePage::initialize(const std::shared_ptr<MediaPlayerMain>& mediaPlayer)
	{
		m_mediaPlayer = mediaPlayer;
		m_playQueue = std::make_shared<PlayQueue>();
		m_mediaPlayer->setPlayQueue(m_playQueue);
	}
}


void winrt::MediaPlayer::implementation::PlayQueuePage::clearButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
	m_mediaPlayer->clearQueue();
}


void winrt::MediaPlayer::implementation::PlayQueuePage::addMediaButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
	PickVideoFileAsync();
}
 
winrt::fire_and_forget winrt::MediaPlayer::implementation::PlayQueuePage::PickVideoFileAsync()
{
	winrt::Windows::Storage::Pickers::FileOpenPicker picker;
	picker.SuggestedStartLocation(winrt::Windows::Storage::Pickers::PickerLocationId::VideosLibrary);
	picker.FileTypeFilter().Append(L".mp4");
	picker.FileTypeFilter().Append(L".avi");
	picker.FileTypeFilter().Append(L".mkv");
	picker.FileTypeFilter().Append(L".mov");

	auto initializeWithWindow = picker.as<::IInitializeWithWindow>();
	HWND hwnd = GetActiveWindow();
	winrt::check_hresult(initializeWithWindow->Initialize(hwnd));

	winrt::Windows::Storage::StorageFile file = co_await picker.PickSingleFileAsync();
	if (file != nullptr)
	{
		auto videoPath = file.Path();
		m_mediaPlayer->addToNextUp(MediaFile(std::wstring(videoPath.c_str())));
		updateMediaQueue();
	}
}

void winrt::MediaPlayer::implementation::PlayQueuePage::updateMediaQueue()
{
	m_mediaQueue.Clear();
	for (const auto& mediaFile : m_playQueue->getQueue())
	{
		m_mediaQueue.Append(winrt::make<winrt::MediaPlayer::implementation::MediaFileViewModel>());
	}
}
