#include "pch.h"
#include "PlaylistsPage.xaml.h"
#if __has_include("PlaylistsPage.g.cpp")
#include "PlaylistsPage.g.cpp"
#endif
#include "PlaylistItemViewModel.h"
#include <PlaylistViewPage.xaml.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MediaPlayer::implementation
{
	PlaylistsPage::PlaylistsPage()
	{
		PlaylistsPageT::InitializeComponent();

		winrt::MediaPlayer::PlaylistItemViewModel playlist(L"playlist with items");
		auto playlistImpl = playlist.try_as<winrt::MediaPlayer::implementation::PlaylistItemViewModel>();
		playlistImpl->Playlist().addFile(MediaFile(L"C:\\Users\\palan\\Downloads\\Fortnite.mp4"));
		playlistImpl->Playlist().addFile(MediaFile(L"C:\\Users\\palan\\Downloads\\coca.mp4"));
		playlistImpl->Playlist().addFile(MediaFile(L"C:\\Users\\palan\\Downloads\\FortHD.mp4"));
		playlistImpl->Playlist().addFile(MediaFile(L"C:\\Users\\palan\\Downloads\\coca.mp4"));
		playlistImpl->Playlist().addFile(MediaFile(L"C:\\Users\\palan\\Downloads\\FortHD.mp4"));
		playlistImpl->Playlist().addFile(MediaFile(L"C:\\Users\\palan\\Downloads\\coca.mp4"));
		items.Append(playlist);
		items.Append(winrt::MediaPlayer::PlaylistItemViewModel(L"Playlist 1"));
		items.Append(winrt::MediaPlayer::PlaylistItemViewModel(L"Playlist 2"));
		items.Append(winrt::MediaPlayer::PlaylistItemViewModel(L"Playlist 3"));
		items.Append(winrt::MediaPlayer::PlaylistItemViewModel(L"Playlist 4"));

		MyGridView().ItemsSource(items);

	}

	void PlaylistsPage::initialize(const std::shared_ptr<MediaPlayerMain>& mediaPlayer, const winrt::Microsoft::UI::Xaml::Controls::Frame& frame)
	{
		m_mediaPlayer = mediaPlayer;
		m_contentFrame = frame;
	}
}

void winrt::MediaPlayer::implementation::PlaylistsPage::MyGridView_ItemClick(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::ItemClickEventArgs const& e)
{
	auto clickedItem = e.ClickedItem().as<winrt::MediaPlayer::PlaylistItemViewModel>();
	auto playlistName = clickedItem.Name();

	auto it = m_playlistPages.find(playlistName);
	
	if (it != m_playlistPages.end())
	{
		m_contentFrame.Content(it->second);
	}
	else
	{
		winrt::MediaPlayer::PlaylistViewPage newPage;
		auto pageImplementation = newPage.as<winrt::MediaPlayer::implementation::PlaylistViewPage>();
		if (pageImplementation)
		{
			pageImplementation->initialize(clickedItem, m_mediaPlayer, m_contentFrame);
		}
		m_playlistPages[playlistName] = newPage;

		m_contentFrame.Content(newPage);
	}
}

void winrt::MediaPlayer::implementation::PlaylistsPage::addPlaylistButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
	NewPlaylistDialog().ShowAsync();
}

void winrt::MediaPlayer::implementation::PlaylistsPage::NewPlaylistDialog_PrimaryButtonClick(winrt::Microsoft::UI::Xaml::Controls::ContentDialog const& sender, winrt::Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const& args)
{
	hstring newPlaylistName = PlaylistNameTextBox().Text();

	if (!newPlaylistName.empty())
	{
		items.Append(winrt::MediaPlayer::PlaylistItemViewModel(newPlaylistName));
	}
}