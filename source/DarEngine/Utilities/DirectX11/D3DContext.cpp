#include "stdafx.h"
#include "D3DContext.h"
#include "Diagnostics/Exception.h"
#include "Utilities/Win32/Window.h"


DE::Utilities::DirectX11::D3DContext::D3DContext(const Win32::Window& window)
{
	InitializeDevice();
	CheckMultiSamplingQualityLevels();
	InitializeSwapChain(window);
	InitializeRenderTargetView();
	InitializeDepthStencilView(window);
	BindViewsToOutputMerger();
	SetupViewPort(window);
}

DE::Utilities::DirectX11::D3DContext::~D3DContext()
{
	if (m_d3dDeviceContext != nullptr)
	{
		m_d3dDeviceContext->ClearState();
	}
}

void DE::Utilities::DirectX11::D3DContext::InitializeDevice()
{
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined( _DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	std::array<D3D_FEATURE_LEVEL, 1> featureLevels = {
		D3D_FEATURE_LEVEL_11_1 ,
	};
	HRESULT hr;
	if (FAILED(hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE,
		NULL, createDeviceFlags, featureLevels.data(), featureLevels.size(),
		D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &m_d3dFeatureLevel,
		m_d3dDeviceContext.GetAddressOf())))
	{
		if (FAILED(hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE,
			NULL, createDeviceFlags, NULL, NULL,
			D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &m_d3dFeatureLevel,
			m_d3dDeviceContext.GetAddressOf())))
		{
			throw Diagnostics::Exception{ L"D3D11CreateDevice() failed" };
		}
	}
}

void DE::Utilities::DirectX11::D3DContext::CheckMultiSamplingQualityLevels()
{
	m_d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM,
		m_d3dMultiSamplingCount, &m_d3dMultiSamplingQualityLevelCount);
	if (m_d3dMultiSamplingQualityLevelCount == 0)
	{
		throw Diagnostics::Exception{ L"Unsupported multi-sampling quality" };
	}
}

void DE::Utilities::DirectX11::D3DContext::InitializeSwapChain(const Win32::Window& window)
{ 
	HRESULT hr;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.Width = window.GetClientAreaWidth();
	swapChainDesc.Height = window.GetClientAreaHeight();
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	if (m_d3disMultiSamplingEnabled)
	{
		swapChainDesc.SampleDesc.Count = m_d3dMultiSamplingCount;
		swapChainDesc.SampleDesc.Quality = m_d3dMultiSamplingQualityLevelCount - 1;
	}
	else
	{
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;	// Double buffering (1 back buffer)
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice{};
	if (FAILED(hr = m_d3dDevice->QueryInterface(__uuidof(IDXGIDevice),
		reinterpret_cast< void**>(dxgiDevice.GetAddressOf()))))
	{
		throw Diagnostics::Exception{ L"ID3D11Device::QueryInterface() failed" };
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter{};
	if (FAILED(hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter),
		reinterpret_cast< void**>(dxgiAdapter.GetAddressOf()))))
	{
		throw Diagnostics::Exception{ L"IDXGIDevice::GetParent() failed" };
	}

	Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory{};
	if (FAILED(hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2),
		reinterpret_cast< void**>(dxgiFactory.GetAddressOf()))))
	{
		throw Diagnostics::Exception{ L"IDXGIAdapter::GetParent() failed" };
	}

	if (FAILED(hr = dxgiFactory->CreateSwapChainForHwnd(dxgiDevice.Get(),
		window.GetHandle(), &swapChainDesc, NULL, NULL, m_dxgiSwapChain.GetAddressOf())))
	{
		throw Diagnostics::Exception{ L"IDXGIFactory2::CreateSwapChainForHwnd() failed" };
	}
}

void DE::Utilities::DirectX11::D3DContext::InitializeRenderTargetView()
{
	HRESULT hr;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer{};
	if (FAILED(hr = m_dxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		reinterpret_cast< void**>(backBuffer.GetAddressOf()))))
	{
		throw Diagnostics::Exception{ L"IDXGISwapChain::GetBuffer() failed." };
	}

	if (FAILED(hr = m_d3dDevice->CreateRenderTargetView(backBuffer.Get(),
		nullptr, m_d3dRenderTargetView.GetAddressOf())))
	{
		throw Diagnostics::Exception{ L"IDXGIDevice::CreateRenderTargetView() failed." };
	}
}

void DE::Utilities::DirectX11::D3DContext::InitializeDepthStencilView(const Win32::Window& window)
{
	HRESULT hr;

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.Width = window.GetClientAreaWidth();
	depthStencilDesc.Height = window.GetClientAreaHeight();
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	if (m_d3disMultiSamplingEnabled)
	{
		depthStencilDesc.SampleDesc.Count = m_d3dMultiSamplingCount;
		depthStencilDesc.SampleDesc.Quality = m_d3dMultiSamplingQualityLevelCount - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	if (FAILED(hr = m_d3dDevice->CreateTexture2D(&depthStencilDesc,
		nullptr, m_d3dDepthStencilBuffer.GetAddressOf())))
	{
		throw Diagnostics::Exception{ L"IDXGIDevice::CreateTexture2D() failed." };
	}
	if (FAILED(hr = m_d3dDevice->CreateDepthStencilView(m_d3dDepthStencilBuffer.Get(), nullptr,
		m_d3dDepthStencilView.GetAddressOf())))
	{
		throw Diagnostics::Exception{ L"IDXGIDevice::CreateDepthStencilView() failed." };
	}
}

void DE::Utilities::DirectX11::D3DContext::BindViewsToOutputMerger()
{
	m_d3dDeviceContext->OMSetRenderTargets(1, m_d3dRenderTargetView.GetAddressOf(), m_d3dDepthStencilView.Get());
}

void DE::Utilities::DirectX11::D3DContext::SetupViewPort(const Win32::Window& window)
{
	m_d3dViewport.TopLeftX = 0.0f;
	m_d3dViewport.TopLeftY = 0.0f;
	m_d3dViewport.Width = static_cast< float>(window.GetClientAreaWidth());
	m_d3dViewport.Height = static_cast< float>(window.GetClientAreaHeight());
	m_d3dViewport.MinDepth = 0.0f;
	m_d3dViewport.MaxDepth = 1.0f;

	m_d3dDeviceContext->RSSetViewports(1, &m_d3dViewport);
}
