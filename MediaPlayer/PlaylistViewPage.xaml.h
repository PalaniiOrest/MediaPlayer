#pragma once

#include "PlaylistViewPage.g.h"
#include "PlaylistItemViewModel.h"
#include "MediaPlayerMain.h"


namespace winrt::MediaPlayer::implementation
{
    struct PlaylistViewPage : PlaylistViewPageT<PlaylistViewPage>
    {
        PlaylistViewPage();

        void initialize(winrt::MediaPlayer::PlaylistItemViewModel const& viewModel, const std::shared_ptr<MediaPlayerMain>& mediaPlayer, const winrt::Microsoft::UI::Xaml::Controls::Frame& frame);

        void OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::MediaPlayer::MediaFileViewModel> PlaylistQueue()
        {
            return m_playlistQueue;
        }
        void updatePlaylistQueue();

    private:
        std::shared_ptr<MediaPlayerMain> m_mediaPlayer;
        winrt::MediaPlayer::PlaylistItemViewModel m_viewModel{ nullptr };
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::MediaPlayer::MediaFileViewModel> m_playlistQueue
        {
            winrt::single_threaded_observable_vector<winrt::MediaPlayer::MediaFileViewModel>()
        };
        winrt::Microsoft::UI::Xaml::Controls::Frame m_frame;
    public:
        void PlaylistListView_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        winrt::fire_and_forget PickVideoFileAsync();
        void BackButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void PlayAllButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void AddToButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void RenameButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void DeleteButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::MediaPlayer::factory_implementation
{
    struct PlaylistViewPage : PlaylistViewPageT<PlaylistViewPage, implementation::PlaylistViewPage>
    {
    };
}
