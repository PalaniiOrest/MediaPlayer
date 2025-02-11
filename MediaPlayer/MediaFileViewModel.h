#pragma once

#include "MediaFileViewModel.g.h"

namespace winrt::MediaPlayer::implementation
{
    struct MediaFileViewModel : MediaFileViewModelT<MediaFileViewModel>
    {
        MediaFileViewModel()
            : m_title(L"***")
            , m_year(L"2005")
            , m_duration(L"26:11")
        { }

        hstring Title() const { return m_title; }
        void Title(hstring const& value) { m_title = value; }

        hstring Year() const { return m_year; }
        void Year(hstring value) { m_year = value; }

        hstring Duration() const { return m_duration; }
        void Duration(hstring const& value) { m_duration = value; }

    private:
        hstring m_title;
        hstring m_year;
        hstring m_duration;
    };
}

namespace winrt::MediaPlayer::factory_implementation
{
    struct MediaFileViewModel : MediaFileViewModelT<MediaFileViewModel, implementation::MediaFileViewModel>
    {
    };
}
