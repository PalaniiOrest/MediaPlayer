#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#include <Microsoft.UI.Xaml.Window.h>
#endif
#include <iostream>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

#undef max
#undef min

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

		UpdateProgressLoopAsync();

		RootGrid().KeyDown({ this, &MainWindow::OnKeyDown });
	}

	void MainWindow::playButton_Click(IInspectable const&, RoutedEventArgs const&)
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

	void MainWindow::OnSelectVideoButtonClick(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		pauseMedia();
		PickVideoFileAsync();
	}

	void MainWindow::OnSwapChainPanelSizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& args)
	{
		auto newSize = args.NewSize();

		m_mediaPlayer->updateSizeDependentResources(newSize.Width, newSize.Height);
	}

	winrt::fire_and_forget MainWindow::PickVideoFileAsync()
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

		ProgressSlider().Value(0);
		ProgressSlider().Maximum(m_mediaPlayer->getVideoDuration() / 10000000.0);

		playButton().Content(box_value(L"Play"));
		m_isPlaying = false;
		m_mediaPlayer->pause();
	}

	winrt::fire_and_forget MainWindow::UpdateProgressLoopAsync()
	{
		using namespace std::chrono_literals;

		auto dispatcherQueue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
		if (!dispatcherQueue)
		{
			co_return;
		}

		while (true) //TODO: change while (true) on something because in the end of program we have exception
		{
			co_await winrt::resume_background();

			if (m_isPlaying && m_mediaPlayer && !m_isSliderUpdate)
			{
				uint64_t currentPosition = m_mediaPlayer->getCurrentPosition();
				uint64_t duration = m_mediaPlayer->getVideoDuration();

				if (duration > 0)
				{
					float progress = static_cast<float>(currentPosition) / duration;

					co_await wil::resume_foreground(dispatcherQueue);;
					m_isSliderUpdate = true;
					ProgressSlider().Value(progress * ProgressSlider().Maximum());
					m_isSliderUpdate = false;
				}
			}

			co_await winrt::resume_after(std::chrono::milliseconds(10));
		}
	}

	void MainWindow::playMedia()
	{
		playButton().Content(box_value(L"Pause"));
		m_isPlaying = true;
		m_mediaPlayer->play();
	}

	void MainWindow::pauseMedia()
	{
		playButton().Content(box_value(L"Play"));
		m_isPlaying = false;
		m_mediaPlayer->pause();
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

void winrt::MediaPlayer::implementation::MainWindow::ProgressSlider_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e)
{
	if (m_isSliderUpdate)
	{
		return;
	}
	m_mediaPlayer->seekToTime(static_cast<uint64_t>(e.NewValue() * 10000000));
}

void winrt::MediaPlayer::implementation::MainWindow::OnKeyDown(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e)
{
	if (e.Key() == winrt::Windows::System::VirtualKey::Space)
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

        e.Handled(true);
    }
	else if (e.Key() == winrt::Windows::System::VirtualKey::Up)
	{
		double newVolume = m_mediaPlayer->getCurrentVolume() + 0.05;

		newVolume = std::min(newVolume, VolumeSlider().Maximum());
		VolumeSlider().Value(newVolume);
	}
	else if (e.Key() == winrt::Windows::System::VirtualKey::Down)
	{
		double newVolume = m_mediaPlayer->getCurrentVolume() - 0.05;

		newVolume = std::max(newVolume, VolumeSlider().Minimum());
		VolumeSlider().Value(newVolume);
	}
}

void winrt::MediaPlayer::implementation::MainWindow::VolumeSlider_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e)
{
	if (m_isPlaying)
	{
		m_mediaPlayer->setVolume(static_cast<double>(e.NewValue()));
	}
}
