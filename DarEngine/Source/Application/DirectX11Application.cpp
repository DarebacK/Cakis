#include "stdafx.h"
#include "DirectX11Application.h"
#include "Resource/Memory.h"
#include <array>

DarEngine::DirectX11Application::DirectX11Application(HINSTANCE instanceHandle, UINT clientAreaWidth, UINT clientAreaHeight, const std::wstring& applicationWindowTitle)
	:Win32Application(instanceHandle, clientAreaWidth, clientAreaHeight, applicationWindowTitle)
{
}

DarEngine::DirectX11Application::~DirectX11Application()
{
	if(immediateDeviceContext)
	{
		immediateDeviceContext->ClearState();
	}
	SafeRelease(renderTargetView);
	SafeRelease(swapChain);
	SafeRelease(immediateDeviceContext);
	SafeRelease(device);
}

void DarEngine::DirectX11Application::OnApplicationInitialization()
{
	if(!InitializeDirect3D())
	{
		QuitApplication(1);
		return;
	}

}

void DarEngine::DirectX11Application::OnMessageLoopTick()
{
	immediateDeviceContext->ClearRenderTargetView(renderTargetView, DirectX::Colors::CornflowerBlue);

	swapChain->Present(0, 0);
}

bool DarEngine::DirectX11Application::InitializeDirect3D()
{
	// CREATE DEVICE AND SWAP CHAIN
	UINT createDeviceFlags{ 0 };

#ifdef _DEBUG 
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif //_DEBUG

	const std::array<D3D_DRIVER_TYPE, 3> driverTypes{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	const std::array<D3D_FEATURE_LEVEL, 2> featureLevels{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	DXGI_SWAP_CHAIN_DESC swapChainDescription{};
	swapChainDescription.BufferCount =				1; // double buffered
	swapChainDescription.BufferDesc.Width =			GetClientAreaWidth();
	swapChainDescription.BufferDesc.Height =		GetClientAreaHeight();
	swapChainDescription.BufferDesc.Format =		DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDescription.BufferUsage =				DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescription.OutputWindow =				GetApplicationWindowHandle();
	swapChainDescription.SwapEffect =				DXGI_SWAP_EFFECT_DISCARD;
	swapChainDescription.Windowed =					true;
	swapChainDescription.SampleDesc.Count =			1;
	swapChainDescription.SampleDesc.Quality =		0;
	swapChainDescription.Flags =					DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allows alt-enter fullscreen toggle

	HRESULT result;
	for(unsigned int i = 0; i < driverTypes.size(); ++i)
	{
		result = D3D11CreateDeviceAndSwapChain(
			nullptr, // default adapter
			driverTypes[i],
			nullptr, // we don't use software module
			createDeviceFlags,
			featureLevels.data(),
			featureLevels.size(),
			D3D11_SDK_VERSION,
			&swapChainDescription,
			&swapChain,
			&device,
			&featureLevel,
			&immediateDeviceContext
		);
		if(SUCCEEDED(result))
		{
			driverType = driverTypes[i];
			break;
		}
	}
	if(FAILED(result))
	{
		return false;
	}

	// CREATE RENDER TARGET VIEW
	ID3D11Texture2D1* backBufferTexture{ nullptr };
	swapChain->GetBuffer(NULL, _uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBufferTexture));
	device->CreateRenderTargetView(backBufferTexture, nullptr, &renderTargetView);

	// BIND RENDER TARGET VIEW
	immediateDeviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);

	// CREATE VIEWPORT
	viewport.Width = static_cast<float>(GetClientAreaWidth());
	viewport.Height = static_cast<float>(GetClientAreaHeight());
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// BIND VIEWPORT
	immediateDeviceContext->RSSetViewports(1, &viewport);

	return true;
}
