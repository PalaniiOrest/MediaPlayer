#include "pch.h"
#include "EffectsControl.xaml.h"
#if __has_include("EffectsControl.g.cpp")
#include "EffectsControl.g.cpp"
#endif



using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MediaPlayer::implementation
{
    int32_t EffectsControl::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void EffectsControl::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    std::set<VideoEffects>& EffectsControl::getVideoEffects()
    {
        applyVideoEffects();
        return m_videoEffects;
    }

    std::set<AudioEffects>& EffectsControl::getAudioEffects()
    {
        applyAudioEffects();
        return m_audioEffects;
    }

}

void winrt::MediaPlayer::implementation::EffectsControl::applyVideoEffects()
{
    if (Grayscale().IsChecked().GetBoolean())
    {
        m_videoEffects.insert(VideoEffects::Grayscale);
    }
    if (Sepia().IsChecked().GetBoolean())
    {
        m_videoEffects.insert(VideoEffects::Sepia);
    }
    if (InvertColors().IsChecked().GetBoolean())
    {
        m_videoEffects.insert(VideoEffects::Negative);
    }
}

void winrt::MediaPlayer::implementation::EffectsControl::applyAudioEffects()
{
    if (Echo().IsChecked().GetBoolean())
    {
        m_audioEffects.insert(AudioEffects::Echo);
    }
    if (BassBoost().IsChecked().GetBoolean())
    {
        m_audioEffects.insert(AudioEffects::BassBoost);
    }
    if (Reverb().IsChecked().GetBoolean())
    {
        m_audioEffects.insert(AudioEffects::Reverse);
    }
    if (GainHalf().IsChecked().GetBoolean())
    {
        m_audioEffects.insert(AudioEffects::GainHalf);
    }
}
