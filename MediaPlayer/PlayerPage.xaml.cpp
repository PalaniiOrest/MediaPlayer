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