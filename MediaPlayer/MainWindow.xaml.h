#pragma once

#include "MainWindow.g.h"
#include <microsoft.ui.xaml.window.h>
#include <DeviceResources.h>
#include "MediaPlayerMain.h"



namespace winrt::MediaPlayer::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
    public:

        MainWindow();
        int32_t MyProperty();
        void MyProperty(int32_t value);

        void init();

        void playButton_Click(IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void OnSelectVideoButtonClick(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void OnSwapChainPanelSizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& args);
        void ProgressSlider_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e);
        void ApplyEffects_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void OnKeyDown(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);

        winrt::fire_and_forget PickVideoFileAsync();
        winrt::fire_and_forget UpdateProgressLoopAsync();


    private:
        void playMedia();
        void pauseMedia();


        std::shared_ptr<::MediaPlayerMain> m_mediaPlayer;
        std::shared_ptr<DeviceResources> m_deviceResources{ nullptr };

        bool m_isPlaying = false;
        bool m_isSliderUpdate = false;
        bool m_isVideoSelected = false;
    public:
        void VolumeSlider_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e);
    };
}

namespace winrt::MediaPlayer::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}