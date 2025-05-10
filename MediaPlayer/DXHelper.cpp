#include "pch.h"
#include "DXHelper.h"
#include <fstream>

std::vector<uint8_t> ReadShaderFile(const std::wstring& filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + std::string(filename.begin(), filename.end()));
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
    {
        throw std::runtime_error("Failed to read file: " + std::string(filename.begin(), filename.end()));
    }

    return buffer;
}
