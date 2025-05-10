#include "pch.h"
#include "Converter.h"
#include <windows.h>

std::wstring Converter::stringToWString(const std::string& str) {
    if (str.empty()) {
        return L"";
    }
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
    wstr.resize(size_needed - 1);
    return wstr;
}

std::string Converter::wstringToString(const std::wstring& wstr) {
    if (wstr.empty()) {
        return "";
    }
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, nullptr, nullptr);
    str.resize(size_needed - 1);
    return str;
}
