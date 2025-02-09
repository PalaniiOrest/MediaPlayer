#pragma once

#include "PlayerPage.g.h"
#include "MediaPlayerMain.h"

namespace winrt::MediaPlayer::implementation
{
    struct PlayerPage : PlayerPageT<PlayerPage>
    {
        PlayerPage() { InitializeComponent(); }

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void initialize(std::shared_ptr<MediaPlayerMain>& mediaPlayer, std::shared_ptr<DeviceResources>& deviceResources, winrt::Microsoft::UI::Xaml::Controls::SwapChainPanel swapChainPanel);

    private:
        std::shared_ptr<DeviceResources> m_deviceResources;
        std::shared_ptr<MediaPlayerMain> m_mediaPlayer;
        winrt::Microsoft::UI::Xaml::Controls::SwapChainPanel m_swapChainPanel;

        bool m_isMinimized = false;
        bool m_isPlaying = false;
        bool m_isVideoSelected = false;
        
    public:
        void MinimizedPlayerPresenter_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& e);
        void OnSelectVideoButtonClick(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void pauseMedia();
        void playMedia();
        void playButton_Click(IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        winrt::fire_and_forget PickVideoFileAsync();
    };
}

namespace winrt::MediaPlayer::factory_implementation
{
    struct PlayerPage : PlayerPageT<PlayerPage, implementation::PlayerPage>
    {
    };
}
