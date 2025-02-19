#include "pch.h"
#include "MediaFile.h"
#include <filesystem>
#include <random>

MediaFile::MediaFile(const std::wstring& filePath)
	: m_filePath(filePath)
	, m_id(std::to_wstring(std::hash<std::wstring>{}(filePath) ^ (std::random_device{}() << 1)))
    , m_duration(calculateVideoDuration(filePath))
{
	m_name = std::filesystem::path(filePath.c_str()).filename().wstring();
}

std::wstring MediaFile::calculateVideoDuration(const std::wstring& filePath)
{
    MFStartup(MF_VERSION);
    winrt::com_ptr<IMFSourceReader> pReader;
    std::wstring durationStr = L"00:00:00";

    HRESULT hr = MFCreateSourceReaderFromURL(filePath.c_str(), nullptr, pReader.put());
    if (SUCCEEDED(hr)) {
        PROPVARIANT var;
        PropVariantInit(&var);

        hr = pReader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &var);
        if (SUCCEEDED(hr)) {
            LONGLONG duration = var.uhVal.QuadPart;
            int totalSeconds = static_cast<int>(duration / 10000000);
            int hours = totalSeconds / 3600;
            int minutes = (totalSeconds % 3600) / 60;
            int seconds = totalSeconds % 60;

            std::wstringstream ss;
            ss << std::setw(2) << std::setfill(L'0') << hours << L":"
                << std::setw(2) << std::setfill(L'0') << minutes << L":"
                << std::setw(2) << std::setfill(L'0') << seconds;
            durationStr = ss.str();
        }
        PropVariantClear(&var);
    }
    return durationStr;
}