#include "pch.h"
#include "MediaFile.h"
#include <filesystem>
#include <random>

MediaFile::MediaFile(const std::wstring& filePath)
	: m_filePath(filePath)
	, m_id(std::to_wstring(std::hash<std::wstring>{}(filePath) ^ (std::random_device{}() << 1)))
{
	m_name = std::filesystem::path(filePath.c_str()).filename().wstring();
}
