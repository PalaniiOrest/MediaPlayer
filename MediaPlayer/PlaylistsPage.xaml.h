#pragma once

#include "PlaylistsPage.g.h"

namespace winrt::MediaPlayer::implementation
{
    struct PlaylistsPage : PlaylistsPageT<PlaylistsPage>
    {
        PlaylistsPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void myButton_Click(IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::MediaPlayer::factory_implementation
{
    struct PlaylistsPage : PlaylistsPageT<PlaylistsPage, implementation::PlaylistsPage>
    {
    };
}
