#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#include <Microsoft.UI.Xaml.Window.h>
#endif
#include <iostream>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MediaPlayer::implementation
{

	MainWindow::MainWindow()
	{
		
	}

	void MainWindow::init()
	{
		m_deviceResources = std::make_shared<DeviceResources>();
		m_deviceResources->SetSwapChainPanel(swapChainPanel());

		m_mediaPlayer = std::make_shared<MediaPlayerMain>(m_deviceResources);
		m_mediaPlayer->startRenderLoop();
	}

	void MainWindow::playButton_Click(IInspectable const&, RoutedEventArgs const&)
	{
		playButton().Content(box_value(L"Clicked"));

		
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
