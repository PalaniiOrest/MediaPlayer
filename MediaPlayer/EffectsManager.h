#pragma once
#include "IVideoEffect.h"
#include "IAudioEffect.h"
#include "EffectsEnums.h"

#undef min

class EffectsManager
{
public:
    void addVideoEffects(VideoFrame& frame);
    void addAudioEffects(AudioFrame& frame);

    void setVideoEffects(std::set<VideoEffects>& effectsList);
    void setAudioEffects(std::set<AudioEffects>& effectsList);
    class NegativeEffect : public IVideoEffect
    {
    public:
        void addTo(VideoFrame& frame) override;
    };
    class SepiaEffect : public IVideoEffect
    {
    public:
        void addTo(VideoFrame& frame) override;
    };
    class GrayscaleEffect : public IVideoEffect
    {
    public:
        void addTo(VideoFrame& frame) override;
    };
    class ReverseEffect : public IAudioEffect
    {
    public:
        void addTo(AudioFrame& frame) override;
    };
    class GainEffect : public IAudioEffect
    {
    public:
        void addTo(AudioFrame& frame) override;
    };
    class EchoEffect : public IAudioEffect
    {
        void addTo(AudioFrame& frame) override;
    };
    static void applyNegativeEffect(BYTE* data, DWORD length) {
        for (DWORD i = 0; i < length; i += 4) {
            data[i] = 255 - data[i];
            data[i + 1] = 255 - data[i + 1];
            data[i + 2] = 255 - data[i + 2];
        }
    }

    static void applySepiaEffect(BYTE* data, DWORD length) {
        for (DWORD i = 0; i < length; i += 4) {
            BYTE blue = data[i];
            BYTE green = data[i + 1];
            BYTE red = data[i + 2];

            BYTE tr = static_cast<BYTE>(std::min(0.393 * red + 0.769 * green + 0.189 * blue, 255.0));
            BYTE tg = static_cast<BYTE>(std::min(0.349 * red + 0.686 * green + 0.168 * blue, 255.0));
            BYTE tb = static_cast<BYTE>(std::min(0.272 * red + 0.534 * green + 0.131 * blue, 255.0));

            data[i] = tb; // Blue
            data[i + 1] = tg; // Green
            data[i + 2] = tr; // Red
        }
    }

    static void applyGrayscaleEffect(BYTE* data, DWORD length) {
        for (DWORD i = 0; i < length; i += 4) {
            BYTE blue = data[i];
            BYTE green = data[i + 1];
            BYTE red = data[i + 2];

            BYTE gray = static_cast<BYTE>(0.299 * red + 0.587 * green + 0.114 * blue);

            data[i] = gray; // Blue
            data[i + 1] = gray; // Green
            data[i + 2] = gray; // Red
        }
    }

    static void applyBrightnessEffect(BYTE* data, DWORD length, int brightness) {
        for (DWORD i = 0; i < length; i += 4) {
            data[i] = static_cast<BYTE>(std::min(data[i] + brightness, 255));     // Blue
            data[i + 1] = static_cast<BYTE>(std::min(data[i + 1] + brightness, 255)); // Green
            data[i + 2] = static_cast<BYTE>(std::min(data[i + 2] + brightness, 255)); // Red
        }
    }

    static void applyReverseEffect(std::vector<uint8_t>& buffer) {
        size_t sampleCount = buffer.size() / 2;
        for (size_t i = 0; i < sampleCount / 2; ++i) {
            size_t j = sampleCount - 1 - i;

            std::swap(buffer[i * 2], buffer[j * 2]);  
            std::swap(buffer[i * 2 + 1], buffer[j * 2 + 1]);
        }
    }

    static void applyGainEffect(std::vector<uint8_t>& buffer, float gainFactor) {
        for (size_t i = 0; i < buffer.size(); i += 2) {
            int16_t* sample = reinterpret_cast<int16_t*>(&buffer[i]);
            int16_t newSample = static_cast<int16_t>(*sample * gainFactor);
            *sample = std::clamp(newSample, static_cast<int16_t>(INT16_MIN), static_cast<int16_t>(INT16_MAX));
        }
    }

    static void applyEchoEffect(std::vector<uint8_t>& buffer, int sampleRate, float delaySeconds, float decayFactor) {
        int delaySamples = static_cast<int>(sampleRate * delaySeconds) * 2;
        for (size_t i = delaySamples; i < buffer.size(); i += 2) {
            int16_t* currentSample = reinterpret_cast<int16_t*>(&buffer[i]);
            int16_t* delayedSample = reinterpret_cast<int16_t*>(&buffer[i - delaySamples]);

            int16_t newSample = static_cast<int16_t>(*currentSample + decayFactor * (*delayedSample));
            *currentSample = std::clamp(newSample, static_cast<int16_t>(INT16_MIN), static_cast<int16_t>(INT16_MAX));
        }
    }

private:
    std::set<std::unique_ptr<IVideoEffect>> m_videoEffects;
    std::set<std::unique_ptr<IAudioEffect>> m_audioEffects;

};

