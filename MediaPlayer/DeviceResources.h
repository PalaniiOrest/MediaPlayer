#pragma once
#include "StreamingVoiceContext.h"

class DeviceResources
{
public:
    DeviceResources();
    virtual ~DeviceResources();
    void init(::winrt::Microsoft::UI::Xaml::Controls::SwapChainPanel swapChainPanel);

    winrt::com_ptr<::ID3D11Device> getD3DDevice() { return m_d3dDevice; }
    winrt::com_ptr<::ID3D11DeviceContext> getD3DDeviceContext() { return m_d3dDeviceContext; }
    winrt::com_ptr<::ID2D1Device> getD2DDevice() { return m_d2dDevice; }
    winrt::com_ptr<::IDXGIDevice> getDXGIDevide() { return m_dxgiDevice; }
    winrt::com_ptr<::ID2D1DeviceContext> getD2DDeviceContext() { return m_d2dDeviceContext; }
    winrt::com_ptr<::IDXGIAdapter> getDXGIAdapter() { return m_dxgiAdapter; }
    winrt::com_ptr<::IDXGIFactory2> getDXGIFactory() { return m_dxgiFactory; }
    winrt::com_ptr<::IDXGISwapChain1> getSwapChain() { return m_swapChain; }

    void updateAudioResources(std::wstring mediaPath);
    IXAudio2SourceVoice* getSourceVoice() { return m_sourceVoice; }
    StreamingVoiceContext& getVoiceContext() { return m_voiceContext; }



protected:
    void initAudioResources();

    winrt::com_ptr<::ID3D11Device> m_d3dDevice;
    winrt::com_ptr<::ID3D11DeviceContext> m_d3dDeviceContext;
    winrt::com_ptr<::ID2D1Device> m_d2dDevice;
    winrt::com_ptr<::ID2D1DeviceContext> m_d2dDeviceContext;
    winrt::com_ptr<::IDXGIDevice> m_dxgiDevice;
    winrt::com_ptr<::IDXGIFactory2> m_dxgiFactory;
    winrt::com_ptr<::IDXGIAdapter> m_dxgiAdapter;
    winrt::com_ptr<::IDXGISwapChain1> m_swapChain;
    winrt::com_ptr<::IDXGISurface> m_dxgiBackBuffer;
    winrt::com_ptr<::ID2D1Bitmap1> m_targetBitmap;

    winrt::com_ptr<IXAudio2> m_xAudio2 = nullptr;
    IXAudio2MasteringVoice* m_masterVoice = nullptr;
    IXAudio2SourceVoice* m_sourceVoice = nullptr;
    StreamingVoiceContext m_voiceContext;
};