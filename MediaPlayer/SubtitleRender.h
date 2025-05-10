#pragma once
#include "SubtitleFrame.h"
#include <ISubtitleDecoder.h>
#include <StepTimer.h>
#include <DeviceResources.h>

class SubtitleRender
{
public:
    SubtitleRender(const std::shared_ptr<DeviceResources>&);

    void update(const StepTimer& timer);
    void render();
    void loadMedia(const MediaFile& media);
    void loadSubtitles(const std::wstring& path);
    void setUpdateTextCallback(std::function<void(std::wstring)> callback);

private:
    std::shared_ptr<DeviceResources> m_deviceResources;
    std::unique_ptr<ISubtitleDecoder> m_decoder;
    std::vector<SubtitleFrame> m_frameList;

    std::wstring m_subtitleText = L"";
    std::function<void(std::wstring)> m_updateTextCallback;

    std::mutex m_mutex;
};
