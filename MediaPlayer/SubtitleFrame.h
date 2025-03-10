#pragma once

class SubtitleFrame
{
public:
    SubtitleFrame(std::wstring text, int64_t startTime, int64_t endTime)
        : m_text(text), m_startTime(startTime), m_endTime(endTime) {
    }

    std::wstring getText() const { return m_text; }
    int64_t getStartTime() const { return m_startTime; }
    int64_t getEndTime() const { return m_endTime; }

private:
    std::wstring m_text;
    int64_t m_startTime;
    int64_t m_endTime;
};