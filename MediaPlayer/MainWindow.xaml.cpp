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
		swapChainPanel().SizeChanged({ this, &MainWindow::OnSwapChainPanelSizeChanged });

		m_deviceResources = std::make_shared<DeviceResources>();
		m_deviceResources->init(swapChainPanel());

		m_mediaPlayer = std::make_shared<MediaPlayerMain>(m_deviceResources);
		m_mediaPlayer->startRenderLoop();
	}

	void MainWindow::playButton_Click(IInspectable const&, RoutedEventArgs const&)
	{
		auto currentContent = unbox_value<hstring>(playButton().Content());

		if (currentContent == L"Play")
		{
			playButton().Content(box_value(L"Pause"));
			m_isPlaying = true;
			m_mediaPlayer->play();
		}
		else
		{
			playButton().Content(box_value(L"Play"));
			m_isPlaying = false;
			m_mediaPlayer->pause();
		}

	}

	void MainWindow::OnSelectVideoButtonClick(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		PickVideoFileAsync();
	}

	void MainWindow::OnSwapChainPanelSizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& args)
	{
		auto newSize = args.NewSize();

		//m_deviceResources->SetLogicalSize(newSize);
	}

	winrt::fire_and_forget MainWindow::PickVideoFileAsync()
	{
		winrt::Windows::Storage::Pickers::FileOpenPicker picker;
		picker.SuggestedStartLocation(winrt::Windows::Storage::Pickers::PickerLocationId::VideosLibrary);
		picker.FileTypeFilter().Append(L".mp4");
		picker.FileTypeFilter().Append(L".avi");
		picker.FileTypeFilter().Append(L".mkv");

		auto initializeWithWindow = picker.as<::IInitializeWithWindow>();
		HWND hwnd = GetActiveWindow(); 
		winrt::check_hresult(initializeWithWindow->Initialize(hwnd));

		winrt::Windows::Storage::StorageFile file = co_await picker.PickSingleFileAsync();
		if (file != nullptr)
		{
			auto videoPath = file.Path();
			m_mediaPlayer->selectVideo(videoPath.c_str());
		}
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
