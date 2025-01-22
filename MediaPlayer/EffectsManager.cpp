#include "pch.h"
#include "EffectsManager.h"

void EffectsManager::addVideoEffects(VideoFrame& frame)
{
    for (auto& effect : m_videoEffects)
    {
        effect->addTo(frame);
    }
}

void EffectsManager::addAudioEffects(AudioFrame& frame)
{
    for (auto& effect : m_audioEffects)
    {
        effect->addTo(frame);
    }
}

void EffectsManager::setVideoEffects(std::set<VideoEffects>& effectsList)
{
    m_videoEffects.clear();
    for (auto effect : effectsList)
    {
        switch (effect)
        {
        case VideoEffects::Grayscale:
            m_videoEffects.insert(std::make_unique<GrayscaleEffect>());
            break;
        case VideoEffects::Sepia:
            m_videoEffects.insert(std::make_unique<SepiaEffect>());
            break;
        case VideoEffects::Negative:
            m_videoEffects.insert(std::make_unique<NegativeEffect>());
            break;
        default:
            break;
        }
    }
}

void EffectsManager::setAudioEffects(std::set<AudioEffects>& effectsList)
{
    m_audioEffects.clear();
    for (auto effect : effectsList)
    {
        switch (effect)
        {
        case AudioEffects::Echo:
            m_audioEffects.insert(std::make_unique<EchoEffect>());
            break;
        case AudioEffects::BassBoost:

            break;
        case AudioEffects::Reverse:
            m_audioEffects.insert(std::make_unique<ReverseEffect>());
            break;
        case AudioEffects::GainHalf:
            m_audioEffects.insert(std::make_unique<GainEffect>());
            break;
        default:
            break;
        }
    }
}

void EffectsManager::NegativeEffect::addTo(VideoFrame& frame)
{
    for (size_t i = 0; i < frame.getBuffer().size(); i += 4) {
        frame.getBuffer()[i] = 255 - frame.getBuffer()[i];
        frame.getBuffer()[i + 1] = 255 - frame.getBuffer()[i + 1];
        frame.getBuffer()[i + 2] = 255 - frame.getBuffer()[i + 2];
    }
}

void EffectsManager::SepiaEffect::addTo(VideoFrame& frame)
{
    for (DWORD i = 0; i < frame.getBuffer().size(); i += 4) {
        BYTE blue = frame.getBuffer()[i];
        BYTE green = frame.getBuffer()[i + 1];
        BYTE red = frame.getBuffer()[i + 2];

        BYTE tr = static_cast<BYTE>(std::min(0.393 * red + 0.769 * green + 0.189 * blue, 255.0));
        BYTE tg = static_cast<BYTE>(std::min(0.349 * red + 0.686 * green + 0.168 * blue, 255.0));
        BYTE tb = static_cast<BYTE>(std::min(0.272 * red + 0.534 * green + 0.131 * blue, 255.0));

        frame.getBuffer()[i] = tb; // Blue
        frame.getBuffer()[i + 1] = tg; // Green
        frame.getBuffer()[i + 2] = tr; // Red
    }
}

void EffectsManager::GrayscaleEffect::addTo(VideoFrame& frame)
{
    for (DWORD i = 0; i < frame.getBuffer().size(); i += 4) {
        BYTE blue = frame.getBuffer()[i];
        BYTE green = frame.getBuffer()[i + 1];
        BYTE red = frame.getBuffer()[i + 2];

        BYTE gray = static_cast<BYTE>(0.299 * red + 0.587 * green + 0.114 * blue);

        frame.getBuffer()[i] = gray; // Blue
        frame.getBuffer()[i + 1] = gray; // Green
        frame.getBuffer()[i + 2] = gray; // Red
    }
}

void EffectsManager::ReverseEffect::addTo(AudioFrame& frame)
{
    size_t sampleCount = frame.getBuffer().size() / 2;
    for (size_t i = 0; i < sampleCount / 2; ++i) {
        size_t j = sampleCount - 1 - i;

        std::swap(frame.getBuffer()[i * 2], frame.getBuffer()[j * 2]);
        std::swap(frame.getBuffer()[i * 2 + 1], frame.getBuffer()[j * 2 + 1]);
    }
}

void EffectsManager::GainEffect::addTo(AudioFrame& frame)
{
    for (size_t i = 0; i < frame.getBuffer().size(); i += 2) {
        int16_t* sample = reinterpret_cast<int16_t*>(&frame.getBuffer()[i]);
        int16_t newSample = static_cast<int16_t>(*sample * 0.5f);
        *sample = std::clamp(newSample, static_cast<int16_t>(INT16_MIN), static_cast<int16_t>(INT16_MAX));
    }
}

void EffectsManager::EchoEffect::addTo(AudioFrame& frame)
{
    int delaySamples = static_cast<int>(44100 * 0.3f) * 2;
    for (size_t i = delaySamples; i < frame.getBuffer().size(); i += 2) {
        int16_t* currentSample = reinterpret_cast<int16_t*>(&frame.getBuffer()[i]);
        int16_t* delayedSample = reinterpret_cast<int16_t*>(&frame.getBuffer()[i - delaySamples]);

        int16_t newSample = static_cast<int16_t>(*currentSample + 0.5f * (*delayedSample));
        *currentSample = std::clamp(
            newSample,
            static_cast<int16_t>(INT16_MIN),
            static_cast<int16_t>(INT16_MAX));
    }
}
