#pragma once

#include "PlaylistViewPage.g.h"
#include "PlaylistItemViewModel.h"

namespace winrt::MediaPlayer::implementation
{
    struct PlaylistViewPage : PlaylistViewPageT<PlaylistViewPage>
    {
        PlaylistViewPage();

        void OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::MediaPlayer::MediaFileViewModel> PlaylistQueue()
        {
            return m_playlistQueue;
        }
        void updatePlaylistQueue();

    private:
        winrt::MediaPlayer::PlaylistItemViewModel m_viewModel{ nullptr };
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::MediaPlayer::MediaFileViewModel> m_playlistQueue
        {
            winrt::single_threaded_observable_vector<winrt::MediaPlayer::MediaFileViewModel>()
        };
    public:
        void PlaylistListView_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
    };
}

namespace winrt::MediaPlayer::factory_implementation
{
    struct PlaylistViewPage : PlaylistViewPageT<PlaylistViewPage, implementation::PlaylistViewPage>
    {
    };
}
