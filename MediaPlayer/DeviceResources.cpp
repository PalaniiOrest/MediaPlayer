#include "pch.h"
#include "DeviceResources.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

DeviceResources::DeviceResources()
{
	initAudioResources();
}

DeviceResources::~DeviceResources()
{
	if (m_sourceVoice) 
	{
		m_sourceVoice->DestroyVoice();
	}
	if (m_masterVoice) 
	{
		m_masterVoice->DestroyVoice();
	}
}

void DeviceResources::init(Controls::SwapChainPanel swapChainPanel)
{
	m_windowWidth = swapChainPanel.ActualWidth();
	m_windowHeight = swapChainPanel.ActualHeight();

	uint32_t creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};


	D3D_FEATURE_LEVEL supportedFeatureLevel;
	winrt::check_hresult(::D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		creationFlags,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		m_d3dDevice.put(),
		&supportedFeatureLevel,
		m_d3dDeviceContext.put())
	);

	m_dxgiDevice = m_d3dDevice.as<::IDXGIDevice>();


	D2D1CreateDevice(m_dxgiDevice.get(), nullptr, m_d2dDevice.put());
	winrt::check_hresult(
		m_d2dDevice->CreateDeviceContext(
			D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
			m_d2dDeviceContext.put()
		)
	);


	m_dxgiDevice->GetAdapter(m_dxgiAdapter.put());

	m_dxgiFactory.capture(m_dxgiAdapter, &IDXGIAdapter::GetParent);

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{ 0 };
	swapChainDesc.Width = 1920;
	swapChainDesc.Height = 1080;
	swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.Flags = 0;


	m_dxgiFactory->CreateSwapChainForComposition(
		m_d3dDevice.get(),
		&swapChainDesc,
		nullptr,
		m_swapChain.put());

	auto panelNative{ swapChainPanel.as<ISwapChainPanelNative>() };

	winrt::check_hresult(
		panelNative->SetSwapChain(m_swapChain.get())
	);

	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96.f,
			96.f
		);


	m_swapChain->GetBuffer(0, __uuidof(m_dxgiBackBuffer), m_dxgiBackBuffer.put_void());

	winrt::check_hresult(
		m_d2dDeviceContext->CreateBitmapFromDxgiSurface(
			m_dxgiBackBuffer.get(),
			&bitmapProperties,
			m_targetBitmap.put()
		)
	);

	m_d2dDeviceContext->SetTarget(m_targetBitmap.get());
}

void DeviceResources::updateSizeDependentResources(uint32_t newWidth, uint32_t newHeight)
{
	m_windowWidth = newWidth;
	m_windowHeight = newHeight;

	m_d2dDeviceContext->SetTarget(nullptr);
	m_targetBitmap = nullptr;
	m_dxgiBackBuffer = nullptr;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	m_swapChain->GetDesc1(&swapChainDesc);
	winrt::check_hresult(m_swapChain->ResizeBuffers(
		swapChainDesc.BufferCount,
		m_windowWidth,
		m_windowHeight,
		swapChainDesc.Format,
		swapChainDesc.Flags
	));

	m_swapChain->GetBuffer(0, __uuidof(m_dxgiBackBuffer), m_dxgiBackBuffer.put_void());

	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96.0f,
			96.0f
		);

	winrt::check_hresult(
		m_d2dDeviceContext->CreateBitmapFromDxgiSurface(
			m_dxgiBackBuffer.get(),
			&bitmapProperties,
			m_targetBitmap.put()
		)
	);

	m_d2dDeviceContext->SetTarget(m_targetBitmap.get());
}

void DeviceResources::initAudioResources()
{
	winrt::com_ptr<IXAudio2> xAudio2;
	HRESULT hr = XAudio2Create(xAudio2.put(), 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to initialize XAudio2.");
	}
	m_xAudio2 = std::move(xAudio2);

	hr = m_xAudio2->CreateMasteringVoice(&m_masterVoice);
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to create mastering voice.");
	}

	WAVEFORMATEX waveFormat = {};
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = 2;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;

	hr = m_xAudio2->CreateSourceVoice(&m_sourceVoice, &waveFormat, 0, 1.0f, &m_voiceContext);
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to create source voice.");
	}
}
