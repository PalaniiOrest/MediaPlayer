#include "pch.h"
#include "PlaylistsPage.xaml.h"
#if __has_include("PlaylistsPage.g.cpp")
#include "PlaylistsPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MediaPlayer::implementation
{
    int32_t PlaylistsPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void PlaylistsPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void PlaylistsPage::myButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        myButton().Content(box_value(L"Clicked"));
    }
}
