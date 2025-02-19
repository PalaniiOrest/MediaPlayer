#pragma once

#include "PlayerPage.g.h"
#include "MediaPlayerMain.h"

namespace winrt::MediaPlayer::implementation
{
    struct PlayerPage : PlayerPageT<PlayerPage>
    {
        PlayerPage() { InitializeComponent(); }

        void initialize(std::shared_ptr<MediaPlayerMain>& mediaPlayer, std::shared_ptr<DeviceResources>& deviceResources, winrt::Microsoft::UI::Xaml::Controls::SwapChainPanel swapChainPanel);
        void OnSwapChainPanelSizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& args);
        void OnKeyDown(winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);

        void playButton_Click(IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void ProgressSlider_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e);
        void VolumeSlider_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e);

        winrt::fire_and_forget UpdateProgressLoopAsync();

        void pauseMedia();
        void playMedia();

        void MinimizedPlayerPresenter_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& e);

    private:
        std::shared_ptr<DeviceResources> m_deviceResources;
        std::shared_ptr<MediaPlayerMain> m_mediaPlayer;
        winrt::Microsoft::UI::Xaml::Controls::SwapChainPanel m_swapChainPanel;

        bool m_isMinimized = false;
        bool m_isPlaying = false;
        bool m_isSliderUpdate = false;
        bool m_isVideoSelected = false;

    public:

        void prevButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void nextButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::MediaPlayer::factory_implementation
{
    struct PlayerPage : PlayerPageT<PlayerPage, implementation::PlayerPage>
    {
    };
}
