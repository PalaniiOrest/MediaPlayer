#pragma once

#include "EffectsControl.g.h"
#include "EffectsEnums.h"

namespace winrt::MediaPlayer::implementation
{
    struct EffectsControl : EffectsControlT<EffectsControl>
    {
        EffectsControl()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        int32_t MyProperty();
        void MyProperty(int32_t value);

        std::set<VideoEffects>& getVideoEffects();
        std::set<AudioEffects>& getAudioEffects();

        std::set<VideoEffects> m_videoEffects;
        std::set<AudioEffects> m_audioEffects;

        void applyVideoEffects();
        void applyAudioEffects();
    };
}

namespace winrt::MediaPlayer::factory_implementation
{
    struct EffectsControl : EffectsControlT<EffectsControl, implementation::EffectsControl>
    {
    };
}
