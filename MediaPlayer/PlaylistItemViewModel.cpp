#include "pch.h"
#include "PlaylistItemViewModel.h"
#if __has_include("PlaylistItemViewModel.g.cpp")
#include "PlaylistItemViewModel.g.cpp"
#endif

namespace winrt::MediaPlayer::implementation
{
    void PlaylistItemViewModel::addMedia(const MediaFile& media)
    {
        m_playlist.addFile(media);
    }
}
