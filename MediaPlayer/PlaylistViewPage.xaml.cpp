#include "pch.h"
#include "PlaylistViewPage.xaml.h"
#if __has_include("PlaylistViewPage.g.cpp")
#include "PlaylistViewPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MediaPlayer::implementation
{
    PlaylistViewPage::PlaylistViewPage()
    {

    }
    void PlaylistViewPage::OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
    {
        if (auto parameter = e.Parameter())
        {
            m_viewModel = parameter.as<winrt::MediaPlayer::PlaylistItemViewModel>();
        }
    }
    void PlaylistViewPage::updatePlaylistQueue()
    {
        m_playlistQueue.Clear();
        // виникла проблема з методом Playlist, його немає тож потрібно додати метод який поверне PlaylistItem 
        // з якого ми вже витягнемо MediaFile і будемо їх відображати

    }
}

void winrt::MediaPlayer::implementation::PlaylistViewPage::PlaylistListView_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
{

}
