#include "pch.h"
#include "PlayerPage.xaml.h"
#if __has_include("PlayerPage.g.cpp")
#include "PlayerPage.g.cpp"
#endif
#include "MainWindow.xaml.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Controls;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MediaPlayer::implementation
{
    int32_t PlayerPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void PlayerPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void PlayerPage::initialize(std::shared_ptr<MediaPlayerMain>& mediaPlayer, std::shared_ptr<DeviceResources>& deviceResources, winrt::Microsoft::UI::Xaml::Controls::SwapChainPanel swapChainPanel)
    {
        m_deviceResources = deviceResources;
        m_mediaPlayer = mediaPlayer;
        m_swapChainPanel = swapChainPanel;
    }
}


void winrt::MediaPlayer::implementation::PlayerPage::MinimizedPlayerPresenter_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& e)
{
    static IPanel originalParent{ nullptr };
    static uint32_t originalIndex = 0;

    if (!originalParent)
    {
        if (auto parent = m_swapChainPanel.Parent())
        {
            originalParent = parent.as<IPanel>();

            uint32_t index;
            if (originalParent.Children().IndexOf(m_swapChainPanel, index))
            {
                originalIndex = index;
            }
        }
    }
    auto mainWindow = MediaPlayer::implementation::MainWindow::Current();

    if (m_isMinimized)
    {
        uint32_t index;
        if (MinimizedPlayerPresenter().Children().IndexOf(m_swapChainPanel, index))
        {
            MinimizedPlayerPresenter().Children().RemoveAt(index);
        }

        if (originalParent)
        {
            if (!originalParent.Children().IndexOf(m_swapChainPanel, index))
            {
                auto size = this->XamlRoot().Size();

                if (originalIndex < originalParent.Children().Size())
                {
                    originalParent.Children().InsertAt(originalIndex, m_swapChainPanel);
                }
                else
                {
                    originalParent.Children().Append(m_swapChainPanel);
                }

                m_deviceResources->updateSizeDependentResources(size.Width, size.Height);

                if (mainWindow)
                {
                    mainWindow->NavigationGrid().Visibility(Visibility::Collapsed);
                    mainWindow->FrameGrid().Visibility(Visibility::Collapsed);
                }
            }
        }

        NameOfMedia().Text(L"Video Name");
        NameOfMedia().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
    }
    if (!m_isMinimized)
    {
        if (originalParent)
        {
            uint32_t index;
            if (originalParent.Children().IndexOf(m_swapChainPanel, index))
            {
                originalParent.Children().RemoveAt(index);
            }
        }
        uint32_t dummyIndex;
        if (!MinimizedPlayerPresenter().Children().IndexOf(m_swapChainPanel, dummyIndex))
        {
            MinimizedPlayerPresenter().Children().Append(m_swapChainPanel);
            m_deviceResources->updateSizeDependentResources(MinimizedPlayerPresenter().Width(), MinimizedPlayerPresenter().Height());
            if (mainWindow)
            {
                mainWindow->NavigationGrid().Visibility(Visibility::Visible);
                mainWindow->FrameGrid().Visibility(Visibility::Visible);
            }
        }
        NameOfMedia().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
    }

    m_isMinimized = !m_isMinimized;
}

void winrt::MediaPlayer::implementation::PlayerPage::OnSelectVideoButtonClick(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
{
    pauseMedia();
    PickVideoFileAsync();
}

void winrt::MediaPlayer::implementation::PlayerPage::pauseMedia()
{
    playButton().Content(box_value(L"Play"));
    m_isPlaying = false;
    m_mediaPlayer->pause();
}

void winrt::MediaPlayer::implementation::PlayerPage::playMedia()
{
    playButton().Content(box_value(L"Pause"));
    m_isPlaying = true;
    m_mediaPlayer->play();
}

void winrt::MediaPlayer::implementation::PlayerPage::playButton_Click(IInspectable const&, RoutedEventArgs const&)
{
    if (m_isVideoSelected)
    {

        if (!m_isPlaying)
        {
            playMedia();
        }
        else
        {
            pauseMedia();
        }
    }
    else
    {
        PickVideoFileAsync();
    }
}

winrt::fire_and_forget winrt::MediaPlayer::implementation::PlayerPage::PickVideoFileAsync()
{
    winrt::Windows::Storage::Pickers::FileOpenPicker picker;
    picker.SuggestedStartLocation(winrt::Windows::Storage::Pickers::PickerLocationId::VideosLibrary);
    picker.FileTypeFilter().Append(L".mp4");
    picker.FileTypeFilter().Append(L".avi");
    picker.FileTypeFilter().Append(L".mkv");
    picker.FileTypeFilter().Append(L".mov");

    auto initializeWithWindow = picker.as<::IInitializeWithWindow>();
    HWND hwnd = GetActiveWindow();
    winrt::check_hresult(initializeWithWindow->Initialize(hwnd));

    winrt::Windows::Storage::StorageFile file = co_await picker.PickSingleFileAsync();
    if (file != nullptr)
    {
        auto videoPath = file.Path();
        m_mediaPlayer->selectVideo(videoPath.c_str());
        m_isVideoSelected = true;
    }

    playButton().Content(box_value(L"Play"));
    m_isPlaying = false;
    m_mediaPlayer->pause();
}