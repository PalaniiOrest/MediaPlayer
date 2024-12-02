#pragma once
#include <mfapi.h>
#include <mfplay.h>
#include <mferror.h>
#include <windows.h>
#include <string>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplay.lib")

class Session
{
public:
	Session(const std::string& filePath);
	~Session();

	bool play();

private:
	bool init();
	

private:
	IMFPMediaPlayer* m_pPlayer = nullptr;
	HRESULT m_hr;
	HWND m_hwnd;
	std::wstring m_filePath;
	bool m_isFullWindow = false;
};

