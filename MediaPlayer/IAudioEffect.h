#pragma once
#include "AudioFrame.h"

class IAudioEffect
{
public:
	virtual void addTo(AudioFrame& frame) = 0;
};

