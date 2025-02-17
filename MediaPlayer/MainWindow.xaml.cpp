#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#include <Microsoft.UI.Xaml.Window.h>
#endif
#include <iostream>
#include <PlayerPage.xaml.h>
#include <PlayQueuePage.xaml.h>
#include <PlaylistsPage.xaml.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

#undef max
#undef min

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MediaPlayer::implementation
{
	MainWindow* MainWindow::m_current = nullptr;

	MainWindow::MainWindow()
	{
			
	}

	MainWindow* winrt::MediaPlayer::implementation::MainWindow::Current()
	{
		return m_current;
	}

	void MainWindow::init()
	{
		m_playQueuePage = winrt::MediaPlayer::PlayQueuePage();
		m_playerPage = winrt::MediaPlayer::PlayerPage();
		PlayerGrid().Children().Append(m_playerPage);

		swapChainPanel().SizeChanged({ this, &MainWindow::OnSwapChainPanelSizeChanged });


		m_deviceResources = std::make_shared<DeviceResources>();
		m_deviceResources->init(swapChainPanel());


		m_mediaPlayer = std::make_shared<MediaPlayerMain>(m_deviceResources);
		m_mediaPlayer->startRenderLoop();

		auto playerPageImpl = m_playerPage.as<winrt::MediaPlayer::implementation::PlayerPage>();
		if (playerPageImpl)
		{
			playerPageImpl->initialize(m_mediaPlayer, m_deviceResources, swapChainPanel());
		}
		auto playQueuePageImpl = m_playQueuePage.as<winrt::MediaPlayer::implementation::PlayQueuePage>();
		if (playQueuePageImpl)
		{
			playQueuePageImpl->initialize(m_mediaPlayer);
		}
		auto playlistsPageImpl = m_playlistsPage.as<winrt::MediaPlayer::implementation::PlaylistsPage>();
		if (playlistsPageImpl)
		{
			playlistsPageImpl->initialize(m_mediaPlayer, ContentFrame());
		}

		RootGrid().KeyDown({ this, &MainWindow::OnKeyDown });

		m_current = this;
	}



	int32_t MainWindow::MyProperty()
	{
		throw hresult_not_implemented();
	}

	void MainWindow::MyProperty(int32_t /* value */)
	{
		throw hresult_not_implemented();
	}
}


void winrt::MediaPlayer::implementation::MainWindow::NavView_SelectionChanged(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args)
{
	if (auto selectedItem = args.SelectedItem())
	{
		auto navItem = selectedItem.as<Controls::NavigationViewItem>();
		auto tag = winrt::unbox_value<hstring>(navItem.Tag());

		if (tag == L"HomePage")
		{
			ContentFrame().Content(m_homePage);
		}
		else if (tag == L"PlaylistsPage")
		{
			ContentFrame().Content(m_playlistsPage);
		}
		else if (tag == L"PlayQueuePage")
		{
			ContentFrame().Content(m_playQueuePage);
		}
	}
}

void winrt::MediaPlayer::implementation::MainWindow::OnKeyDown(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e)
{
	auto playerImpl = m_playerPage.as<winrt::MediaPlayer::implementation::PlayerPage>();
	playerImpl->OnKeyDown(e);
}

void winrt::MediaPlayer::implementation::MainWindow::OnSwapChainPanelSizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& args)
{
	auto playerImpl = m_playerPage.as<winrt::MediaPlayer::implementation::PlayerPage>();
	playerImpl->OnSwapChainPanelSizeChanged(sender, args);

}



