#pragma once

#include "MediaFileViewModel.g.h"
#include "MediaFile.h"

namespace winrt::MediaPlayer::implementation
{
    struct MediaFileViewModel : MediaFileViewModelT<MediaFileViewModel>
    {
        MediaFileViewModel()
            : m_mediaFile(MediaFile(L""))
        {}
        MediaFileViewModel(const MediaFile& mediaFile)
            : m_mediaFile(mediaFile)
        { }

        hstring Id() const { return hstring(m_mediaFile.m_id); }
        void Id(hstring const& value) { m_mediaFile.m_id = value.c_str(); }

        hstring Name() const { return hstring(m_mediaFile.m_name); }
        void Name(hstring const& value) { m_mediaFile.m_name = value.c_str(); }

        hstring Year() const { return hstring(m_mediaFile.m_year); }
        void Year(hstring value) { m_mediaFile.m_year = value.c_str(); }

        hstring Duration() const { return hstring(m_mediaFile.m_duration); }
        void Duration(hstring const& value) { m_mediaFile.m_duration = value.c_str(); }

        hstring MediaPath() const { return hstring(m_mediaFile.m_filePath); }
        void MediaPath(hstring const& value) { m_mediaFile.m_filePath = value.c_str(); }

    private:
        std::wstring calculateVideoDuration(const std::wstring& filePath);
        MediaFile m_mediaFile;
    };
}

namespace winrt::MediaPlayer::factory_implementation
{
    struct MediaFileViewModel : MediaFileViewModelT<MediaFileViewModel, implementation::MediaFileViewModel>
    {
    };
}
