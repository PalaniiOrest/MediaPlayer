#include "pch.h"
#include "PlaylistViewPage.xaml.h"
#if __has_include("PlaylistViewPage.g.cpp")
#include "PlaylistViewPage.g.cpp"
#endif
#include <MediaFileViewModel.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MediaPlayer::implementation
{
    PlaylistViewPage::PlaylistViewPage()
    {

    }
    void PlaylistViewPage::initialize(winrt::MediaPlayer::PlaylistItemViewModel const& viewModel, const std::shared_ptr<MediaPlayerMain>& mediaPlayer, const winrt::Microsoft::UI::Xaml::Controls::Frame& frame)
    {
        m_viewModel = viewModel;
        m_mediaPlayer = mediaPlayer;
        m_frame = frame;

        updatePlaylistQueue();
    }
    void PlaylistViewPage::OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
    {
        if (auto parameter = e.Parameter())
        {
            m_viewModel = parameter.as<winrt::MediaPlayer::PlaylistItemViewModel>();
        }
        updatePlaylistQueue();
    }
    void PlaylistViewPage::updatePlaylistQueue()
    {
        m_playlistQueue.Clear();
        if (auto viewModel = m_viewModel.try_as<winrt::MediaPlayer::implementation::PlaylistItemViewModel>())
        {
            PlaylistItem& playlist = viewModel->Playlist();
            for (const auto& mediaFile : playlist.getPlaylist())
            {
                m_playlistQueue.Append(winrt::make<winrt::MediaPlayer::implementation::MediaFileViewModel>(mediaFile));
            }
        }

    }
}

void winrt::MediaPlayer::implementation::PlaylistViewPage::BackButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
    if (auto frame = this->Frame())
    {
        if (frame.CanGoBack())
        {
            frame.GoBack();
        }
    }
}

void winrt::MediaPlayer::implementation::PlaylistViewPage::PlayAllButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
    m_mediaPlayer->clearQueue();

    if (auto viewModel = m_viewModel.try_as<winrt::MediaPlayer::implementation::PlaylistItemViewModel>())
    {
        PlaylistItem playlist = viewModel->Playlist();
        m_mediaPlayer->addToNextUp(playlist);
    }
}

void winrt::MediaPlayer::implementation::PlaylistViewPage::AddToButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
    PickVideoFileAsync();
}

void winrt::MediaPlayer::implementation::PlaylistViewPage::RenameButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{

}

void winrt::MediaPlayer::implementation::PlaylistViewPage::DeleteButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
    if (auto viewModel = m_viewModel.try_as<winrt::MediaPlayer::implementation::PlaylistItemViewModel>())
    {
        PlaylistItem& playlist = viewModel->Playlist();
        playlist.getPlaylist().clear();
    }
    updatePlaylistQueue();
}

void winrt::MediaPlayer::implementation::PlaylistViewPage::PlaylistListView_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
{
    m_mediaPlayer->clearQueue();
    auto listView = sender.as<winrt::Microsoft::UI::Xaml::Controls::ListView>();
    auto selectedItem = listView.SelectedItem();
    if (selectedItem)
    {
        auto mediaFileImpl = selectedItem.as<winrt::MediaPlayer::implementation::MediaFileViewModel>();
        if (mediaFileImpl)
        {
            if (auto playlistItemImpl = m_viewModel.try_as<winrt::MediaPlayer::implementation::PlaylistItemViewModel>())
            {
                PlaylistItem playlist = playlistItemImpl->Playlist();
                m_mediaPlayer->addToNextUp(playlist);
                m_mediaPlayer->playMediaById(mediaFileImpl->Id().c_str());
            }
        }
    }
}

winrt::fire_and_forget winrt::MediaPlayer::implementation::PlaylistViewPage::PickVideoFileAsync()
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
        MediaFile mediaFile(std::wstring(videoPath.c_str()));
        if (auto viewModel = m_viewModel.try_as<winrt::MediaPlayer::implementation::PlaylistItemViewModel>())
        {
            viewModel->addMedia(mediaFile);
            updatePlaylistQueue();
        }
    }
}