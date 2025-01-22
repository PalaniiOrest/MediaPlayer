#pragma once
#include "VideoFrame.h"

class IVideoEffect
{
public:
	virtual void addTo(VideoFrame& frame) = 0;
};

