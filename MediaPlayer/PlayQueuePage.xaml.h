#pragma once

#include "PlayQueuePage.g.h"
#include "MediaPlayerMain.h"
#include "PlayQueue.h"

namespace winrt::MediaPlayer::implementation
{
    struct PlayQueuePage : PlayQueuePageT<PlayQueuePage>
    {
        PlayQueuePage()
        {
        }

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void initialize(const std::shared_ptr<MediaPlayerMain>& mediaPlayer);

        void clearButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void addMediaButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        winrt::fire_and_forget PickVideoFileAsync();

    private:
        std::shared_ptr<MediaPlayerMain> m_mediaPlayer;
        std::shared_ptr<PlayQueue> m_playQueue;
    };
}

namespace winrt::MediaPlayer::factory_implementation
{
    struct PlayQueuePage : PlayQueuePageT<PlayQueuePage, implementation::PlayQueuePage>
    {
    };
}
