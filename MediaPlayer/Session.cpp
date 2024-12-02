#include "pch.h"
#include "Session.h"
#include "Converter.h"
#include <iostream>


Session::Session(const std::string& filePath)
	: m_filePath(Converter::stringToWString(filePath))
{
	if (init())
	{
		std::cerr << "ERROR\n";
	}
}

Session::~Session()
{
	if (m_pPlayer) {
		m_pPlayer->Shutdown();
		m_pPlayer->Release();
	}
	MFShutdown();
}

bool Session::init()
{
	m_hr = MFStartup(MF_VERSION);
	if (FAILED(m_hr)) {
		wprintf(L"Failed to initialize Media Foundation. Error: 0x%08x\n", m_hr);
		return false;
	}

	m_hr = MFPCreateMediaPlayer(
		m_filePath.c_str(),
		m_isFullWindow,
		0,
		nullptr,
		nullptr,
		&m_pPlayer
	);
	if (SUCCEEDED(m_hr))
	{
		return false;
	}

	return true;
}

bool Session::play()
{

	m_hr = m_pPlayer->Play();
	if SUCCEEDED(m_hr)
	{
		wprintf(L"Playing media file: %s\n",
			m_filePath);
		Sleep(5000);
		return true;

	}
	return false;
}
