#pragma once
#include <cstdint>

constexpr uint64_t TICKS_PER_SECOND = 10'000'000;

enum class DECODER
{
	MEDIA_FOUNDATION,
	FFMPEG
};