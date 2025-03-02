#include "pch.h"
#include "PlayerPage.xaml.h"
#if __has_include("PlayerPage.g.cpp")
#include "PlayerPage.g.cpp"
#endif
#include "MainWindow.xaml.h"
#include "Constants.h"

#undef min
#undef max

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Controls;

namespace winrt::MediaPlayer::implementation
{
	void PlayerPage::initialize(std::shared_ptr<MediaPlayerMain>& mediaPlayer, std::shared_ptr<DeviceResources>& deviceResources, winrt::Microsoft::UI::Xaml::Controls::SwapChainPanel swapChainPanel)
	{
		m_deviceResources = deviceResources;
		m_mediaPlayer = mediaPlayer;
		m_swapChainPanel = swapChainPanel;
		UpdateProgressLoopAsync();
	}
	void PlayerPage::OnSwapChainPanelSizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& args)
	{
		auto newSize = args.NewSize();

		m_mediaPlayer->updateSizeDependentResources(newSize.Width, newSize.Height);
	}
	void PlayerPage::OnKeyDown(winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e)
	{
		if (e.Key() == winrt::Windows::System::VirtualKey::Space)
		{

			if (!m_isPlaying)
			{
				playMedia();
				RootGrid().Visibility(Visibility::Collapsed);
			}
			else
			{
				pauseMedia();
				RootGrid().Visibility(Visibility::Visible);
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
		else if (e.Key() == winrt::Windows::System::VirtualKey::Right)
		{

		}
		else if (e.Key() == winrt::Windows::System::VirtualKey::Left)
		{

		}
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

				m_mediaPlayer->updateSizeDependentResources(size.Width, size.Height);

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
			m_mediaPlayer->updateSizeDependentResources(MinimizedPlayerPresenter().Width(), MinimizedPlayerPresenter().Height());
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
	if (!m_mediaPlayer->isQueueEmpty())
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
}

void winrt::MediaPlayer::implementation::PlayerPage::ProgressSlider_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e)
{
	if (m_isSliderUpdate)
	{
		return;
	}
	m_mediaPlayer->pause();
	m_mediaPlayer->seekToTime(static_cast<uint64_t>(e.NewValue() * 10000000));
	m_mediaPlayer->play();

}

void winrt::MediaPlayer::implementation::PlayerPage::VolumeSlider_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e)
{
	if (m_isPlaying)
	{
		m_mediaPlayer->setVolume(static_cast<double>(e.NewValue()));
	}
}

void winrt::MediaPlayer::implementation::PlayerPage::prevButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
	m_mediaPlayer->playPreviousMedia();
}

void winrt::MediaPlayer::implementation::PlayerPage::nextButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
	m_mediaPlayer->playNextMedia();
}

winrt::fire_and_forget winrt::MediaPlayer::implementation::PlayerPage::UpdateProgressLoopAsync()
{
	using namespace std::chrono_literals;

	auto dispatcherQueue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
	if (!dispatcherQueue)
	{
		co_return;
	}

	while (!m_mediaPlayer->getIsEndOfMedia())
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

		co_await winrt::resume_after(std::chrono::milliseconds(1000));
	}
}

void winrt::MediaPlayer::implementation::PlayerPage::screenshotButton_Click(
	winrt::Windows::Foundation::IInspectable const& sender,
	winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
	ShowScreenshotDialog();
}

winrt::fire_and_forget MediaPlayer::implementation::PlayerPage::ShowScreenshotDialog()
{
	pauseMedia();
	auto strongThis = get_strong();
	ContentDialog dialog;
	dialog.Title(box_value(L"Screenshot"));

	StackPanel panel;

	TextBox fileNameBox;
	fileNameBox.PlaceholderText(L"Enter name...");
	panel.Children().Append(fileNameBox);

	ComboBox formatBox;
	formatBox.Items().Append(box_value(L"PNG"));
	formatBox.Items().Append(box_value(L"JPEG"));
	formatBox.SelectedIndex(0);
	panel.Children().Append(formatBox);

	StackPanel dirPanel;
	dirPanel.Orientation(Orientation::Horizontal);

	TextBox directoryBox;
	directoryBox.IsReadOnly(true);
	directoryBox.Width(250);

	Button browseButton;
	browseButton.Content(box_value(L"Select"));

	browseButton.Click([this, &directoryBox](IInspectable const&, RoutedEventArgs const&) -> winrt::fire_and_forget
		{
			winrt::Windows::Storage::Pickers::FolderPicker folderPicker;
			folderPicker.SuggestedStartLocation(winrt::Windows::Storage::Pickers::PickerLocationId::Desktop);
			folderPicker.FileTypeFilter().Append(L"*");

			auto initializeWithWindow = folderPicker.as<::IInitializeWithWindow>();
			HWND hwnd = GetActiveWindow();
			winrt::check_hresult(initializeWithWindow->Initialize(hwnd));

			auto folder = co_await folderPicker.PickSingleFolderAsync();
			if (folder)
			{
				directoryBox.Text(folder.Path());
			}
		});

	dirPanel.Children().Append(directoryBox);
	dirPanel.Children().Append(browseButton);
	panel.Children().Append(dirPanel);

	dialog.Content(panel);
	dialog.PrimaryButtonText(L"OK");
	dialog.CloseButtonText(L"Cancel");
	dialog.XamlRoot(screenshotButton().XamlRoot());

	auto result = co_await dialog.ShowAsync();

	if (result == ContentDialogResult::Primary)
	{
		std::wstring fileName = fileNameBox.Text().c_str();
		std::wstring directory = directoryBox.Text().c_str();

		if (fileName.empty() || directory.empty())
		{
			ContentDialog errorDialog;
			errorDialog.Title(box_value(L"Error"));
			errorDialog.Content(box_value(L"Please, enter name and select directory."));
			errorDialog.CloseButtonText(L"OK");
			errorDialog.XamlRoot(screenshotButton().XamlRoot());
			co_await errorDialog.ShowAsync();
			co_return;
		}

		std::wstring formatStr = formatBox.SelectedIndex() == 0 ? L".png" : L".jpg";
		GUID format = formatBox.SelectedIndex() == 0 ? GUID_ContainerFormatPng : GUID_ContainerFormatJpeg;

		std::wstring filePath = directory + L"\\" + fileName + formatStr;

		if (m_mediaPlayer)
		{
			m_mediaPlayer->saveCurrentFrameAsScreenshot(filePath, format);
		}
	}
	playMedia();
}
