#pragma once
#include "DeviceResources.h"

class AudioFrame
{
public:
	AudioFrame(const std::shared_ptr<DeviceResources>& deviceResources);
	~AudioFrame();

	void setAudioData(const std::vector<byte>& audioData);
	void play();
	void stop();
	bool isPlaying();

	std::vector<uint8_t>& getBuffer() { return m_buffers[m_currentStreamBuffer]; }

	operator bool() const { return m_buffers[m_currentStreamBuffer].size() > 0; }
private:
	std::shared_ptr<DeviceResources> m_deviceResources;
	
	int m_currentStreamBuffer = 0;
	std::vector<std::vector<uint8_t>> m_buffers;
	unsigned long m_sampleBufferLenght = 0;
};