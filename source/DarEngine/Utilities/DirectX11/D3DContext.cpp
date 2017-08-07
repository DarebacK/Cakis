#include "stdafx.h"
#include "D3DContext.h"
#include "Diagnostics/Exception.h"

DE::Utilities::DirectX11::D3DContext::D3DContext(const HWND windowHandle, int clientAreaWidth, int clientAreaHeight)
{
	if(!windowHandle)
	{
		throw Diagnostics::Exception{ L"D3DContext(HWND, int, int) failed, windowHandle is nullptr." };
	}

	InitializeDevice();
	CheckMultiSamplingQualityLevels();
	InitializeSwapChain(windowHandle, clientAreaWidth, clientAreaHeight);
	InitializeRenderTargetView();
	InitializeDepthStencilView(clientAreaWidth, clientAreaHeight);
	BindViewsToOutputMerger();
	SetupViewPort(clientAreaWidth, clientAreaHeight);
}

DE::Utilities::DirectX11::D3DContext::~D3DContext()
{
	if (m_deviceContext != nullptr)
	{
		m_deviceContext->ClearState();
	}
}

void DE::Utilities::DirectX11::D3DContext::Clear(const DirectX::XMVECTORF32& color)
{
	m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), color);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DE::Utilities::DirectX11::D3DContext::Present()
{
	if(FAILED(m_swapChain->Present(0, 0)))
	{
		throw Diagnostics::Exception{ L"IDXGISwapChain1::Present() failed" };
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

	if (FAILED(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE,
		NULL, createDeviceFlags, featureLevels.data(), featureLevels.size(),
		D3D11_SDK_VERSION, m_device.GetAddressOf(), &m_featureLevel,
		m_deviceContext.GetAddressOf())))
	{
		if (FAILED(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE,
			NULL, createDeviceFlags, NULL, NULL,
			D3D11_SDK_VERSION, m_device.GetAddressOf(), &m_featureLevel,
			m_deviceContext.GetAddressOf())))
		{
			throw Diagnostics::Exception{ L"D3D11CreateDevice() failed" };
		}
	}
}

void DE::Utilities::DirectX11::D3DContext::CheckMultiSamplingQualityLevels()
{
	m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM,
		m_multiSamplingCount, &m_multiSamplingQualityLevelCount);
	if (m_multiSamplingQualityLevelCount == 0)
	{
		throw Diagnostics::Exception{ L"Unsupported multi-sampling quality" };
	}
}

void DE::Utilities::DirectX11::D3DContext::InitializeSwapChain(const HWND windowHandle, int clientAreaWidth, int clientAreaHeight)
{ 
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.Width = clientAreaWidth;
	swapChainDesc.Height = clientAreaHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	if (m_isMultiSamplingEnabled)
	{
		swapChainDesc.SampleDesc.Count = m_multiSamplingCount;
		swapChainDesc.SampleDesc.Quality = m_multiSamplingQualityLevelCount - 1;
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
	if (FAILED(m_device->QueryInterface(__uuidof(IDXGIDevice),
		reinterpret_cast< void**>(dxgiDevice.GetAddressOf()))))
	{
		throw Diagnostics::Exception{ L"ID3D11Device::QueryInterface() failed" };
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter{};
	if (FAILED(dxgiDevice->GetParent(__uuidof(IDXGIAdapter),
		reinterpret_cast< void**>(dxgiAdapter.GetAddressOf()))))
	{
		throw Diagnostics::Exception{ L"IDXGIDevice::GetParent() failed" };
	}

	Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory{};
	if (FAILED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory2),
		reinterpret_cast< void**>(dxgiFactory.GetAddressOf()))))
	{
		throw Diagnostics::Exception{ L"IDXGIAdapter::GetParent() failed" };
	}

	if (FAILED(dxgiFactory->CreateSwapChainForHwnd(dxgiDevice.Get(),
		windowHandle, &swapChainDesc, NULL, NULL, m_swapChain.GetAddressOf())))
	{
		throw Diagnostics::Exception{ L"IDXGIFactory2::CreateSwapChainForHwnd() failed" };
	}
}

void DE::Utilities::DirectX11::D3DContext::InitializeRenderTargetView()
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer{};
	if (FAILED( m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		reinterpret_cast< void**>(backBuffer.GetAddressOf()))))
	{
		throw Diagnostics::Exception{ L"IDXGISwapChain::GetBuffer() failed." };
	}

	if (FAILED(m_device->CreateRenderTargetView(backBuffer.Get(),
		nullptr, m_renderTargetView.GetAddressOf())))
	{
		throw Diagnostics::Exception{ L"IDXGIDevice::CreateRenderTargetView() failed." };
	}
}

void DE::Utilities::DirectX11::D3DContext::InitializeDepthStencilView(int clientAreaWidth, int clientAreaHeight)
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.Width = clientAreaWidth;
	depthStencilDesc.Height = clientAreaHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	if (m_isMultiSamplingEnabled)
	{
		depthStencilDesc.SampleDesc.Count = m_multiSamplingCount;
		depthStencilDesc.SampleDesc.Quality = m_multiSamplingQualityLevelCount - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	if (FAILED(m_device->CreateTexture2D(&depthStencilDesc,
		nullptr, m_depthStencilBuffer.GetAddressOf())))
	{
		throw Diagnostics::Exception{ L"IDXGIDevice::CreateTexture2D() failed." };
	}
	if (FAILED(m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), nullptr,
		m_depthStencilView.GetAddressOf())))
	{
		throw Diagnostics::Exception{ L"IDXGIDevice::CreateDepthStencilView() failed." };
	}
}

void DE::Utilities::DirectX11::D3DContext::BindViewsToOutputMerger()
{
	m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
}

void DE::Utilities::DirectX11::D3DContext::SetupViewPort(int clientAreaWidth, int clientAreaHeight)
{
	m_viewPort.TopLeftX = 0.0f;
	m_viewPort.TopLeftY = 0.0f;
	m_viewPort.Width = static_cast< float>(clientAreaWidth);
	m_viewPort.Height = static_cast< float>(clientAreaHeight);
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;

	m_deviceContext->RSSetViewports(1, &m_viewPort);
}
