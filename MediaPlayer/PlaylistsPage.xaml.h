#pragma once

#include "PlaylistsPage.g.h"
#include "MediaPlayerMain.h"

namespace winrt::MediaPlayer::implementation
{
    struct PlaylistsPage : PlaylistsPageT<PlaylistsPage>
    {
        PlaylistsPage();

        void initialize(const std::shared_ptr<MediaPlayerMain>& mediaPlayer, const winrt::Microsoft::UI::Xaml::Controls::Frame& frame);

        void MyGridView_ItemClick(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::ItemClickEventArgs const& e);

        std::shared_ptr<MediaPlayerMain> m_mediaPlayer;
        winrt::Microsoft::UI::Xaml::Controls::Frame m_contentFrame;
        

        winrt::Windows::Foundation::Collections::IObservableVector<winrt::MediaPlayer::PlaylistItemViewModel> items = single_threaded_observable_vector<winrt::MediaPlayer::PlaylistItemViewModel>();
        
        void addPlaylistButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NewPlaylistDialog_PrimaryButtonClick(winrt::Microsoft::UI::Xaml::Controls::ContentDialog const& sender, winrt::Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const& args);
    };
}

namespace winrt::MediaPlayer::factory_implementation
{
    struct PlaylistsPage : PlaylistsPageT<PlaylistsPage, implementation::PlaylistsPage>
    {
    };
}
