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
        static MainWindow* Current();
        int32_t MyProperty();
        void MyProperty(int32_t value);

        void init();
        void NavView_SelectionChanged(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);



    private:
        static MainWindow* m_current;

        std::shared_ptr<::MediaPlayerMain> m_mediaPlayer;
        std::shared_ptr<DeviceResources> m_deviceResources{ nullptr };

        winrt::MediaPlayer::HomePage m_homePage;
        winrt::MediaPlayer::PlaylistsPage m_playlistsPage;
        winrt::MediaPlayer::PlayQueuePage m_playQueuePage;
        winrt::MediaPlayer::PlayerPage m_playerPage;
    };
}

namespace winrt::MediaPlayer::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}