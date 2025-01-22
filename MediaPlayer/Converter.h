#pragma once
#include <string>

class Converter
{
public:
	static std::wstring stringToWString(const std::string& str);
	static std::string wstringToString(const std::wstring& wstr);
};

