#include "stdafx.h"
#include "Game.h"
#include "Exception.h"

void DE::Game::Run()
{
	try
	{
		Initialize();
		RunGameLoop();
	}
	catch (Exception ex)
	{
		MessageBox(m_window.GetHandle(), ex.message.c_str(), L"Fatal error", MB_ABORTRETRYIGNORE);
		throw;	
	}

	Shutdown(); //TODO: call Shutdown before throw or not?
}

void DE::Game::Exit()
{
	m_isRunning = false;
}

DE::Game::Game(HINSTANCE instanceHandle, const std::wstring& windowTitle, int showCommand)
	:m_instanceHandle {instanceHandle}, m_window{instanceHandle, windowTitle, showCommand}
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
}

void DE::Game::Initialize()
{
	m_window.Show();
	m_clock.Reset();
	InitializeD3D();
}

void DE::Game::InitializeD3D()
{
	//DEVICE CREATION
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
			throw Exception{ L"D3D11CreateDevice() failed" };
		}	
	}

	m_d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM,
		m_d3dMultiSamplingCount, &m_d3dMultiSamplingQualityLevelCount);
	if (m_d3dMultiSamplingQualityLevelCount == 0)
	{
		throw Exception{ L"Unsupported multi-sampling quality" };
	}

	// SWAP CHAIN
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.Width = m_window.GetClientAreaWidth();
	swapChainDesc.Height = m_window.GetClientAreaHeight();
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
		throw Exception{ L"ID3D11Device::QueryInterface() failed"};
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter{};
	if (FAILED(hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter),
		reinterpret_cast< void**>(dxgiAdapter.GetAddressOf()))))
	{
		throw Exception{L"IDXGIDevice::GetParent() failed" };
	}

	Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory{};
	if (FAILED(hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2),
		reinterpret_cast< void**>(dxgiFactory.GetAddressOf()))))
	{
		throw Exception{ L"IDXGIAdapter::GetParent() failed" };
	}

	Microsoft::WRL::ComPtr<IDXGISwapChain1> dxgiSwapChain{};
	if (FAILED(hr = dxgiFactory->CreateSwapChainForHwnd(dxgiDevice.Get(),
		m_window.GetHandle(), &swapChainDesc, NULL, NULL, dxgiSwapChain.GetAddressOf())))
	{
		throw Exception{ L"IDXGIFactory2::CreateSwapChainForHwnd() failed" };
	}

	//RENDER TARGET VIEW
}

void DE::Game::RunGameLoop()
{
	m_isRunning = true;

	MSG message;
	ZeroMemory(&message, sizeof(message));

	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			m_clock.Update();
			auto deltaTime = ComputeDeltaTime(m_clock);
			//TODO: update and draw
		}
	}
}

void DE::Game::Shutdown()
{
	//TODO: implement
}
