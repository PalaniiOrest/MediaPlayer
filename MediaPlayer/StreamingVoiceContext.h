#pragma once

struct StreamingVoiceContext : public IXAudio2VoiceCallback
{
    STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32) override
    {
    }
    STDMETHOD_(void, OnVoiceProcessingPassEnd)() override
    {
    }
    STDMETHOD_(void, OnStreamEnd)() override 
    {
    }
    STDMETHOD_(void, OnBufferStart)(void*) override
    {
    }
    STDMETHOD_(void, OnBufferEnd)(void*) override
    {
        SetEvent(hBufferEndEvent);
    }
    STDMETHOD_(void, OnLoopEnd)(void*) override
    {
    }
    STDMETHOD_(void, OnVoiceError)(void*, HRESULT) override
    {
    }

    HANDLE hBufferEndEvent;

    StreamingVoiceContext() :
#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
        hBufferEndEvent(CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE))
#else
        hBufferEndEvent(CreateEvent(nullptr, FALSE, FALSE, nullptr))
#endif
    {
    }
    virtual ~StreamingVoiceContext()
    {
        CloseHandle(hBufferEndEvent);
    }
};