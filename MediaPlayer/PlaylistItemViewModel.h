#pragma once

#include "PlaylistItemViewModel.g.h"
#include "PlaylistItem.h"

namespace winrt::MediaPlayer::implementation
{
    struct PlaylistItemViewModel : PlaylistItemViewModelT<PlaylistItemViewModel>
    {
        PlaylistItemViewModel()
        {
        }

        PlaylistItemViewModel(const hstring& name)
        {
            m_playlist.setName(name.c_str());
        }

        void Name(const hstring & value) { m_playlist.setName(value.c_str()); }
        hstring Name() { return hstring(m_playlist.getName()); }

        PlaylistItem& Playlist() { return m_playlist; }
        void addMedia(const MediaFile& media);

        private:
            PlaylistItem m_playlist;
    };
}

namespace winrt::MediaPlayer::factory_implementation
{
    struct PlaylistItemViewModel : PlaylistItemViewModelT<PlaylistItemViewModel, implementation::PlaylistItemViewModel>
    {
    };
}
