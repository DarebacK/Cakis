#include "D3D11Renderer.h"
#include "DarEngine.h"
#include <Windows.h>
#include <d3d11_4.h>
#include <wrl.h>

namespace
{
  ID3D11Device* device = nullptr;
  ID3D11DeviceContext* deviceContext = nullptr;
  D3D_FEATURE_LEVEL featureLevel = (D3D_FEATURE_LEVEL)0;
  UINT multisamplingCount = 8;
  IDXGISwapChain1* swapchain = nullptr;

}

bool initD3D11Renderer(HWND window)
{
  // DEVICE
	UINT createDeviceFlags = 0;
  #ifdef DAR_DEBUG
	  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
  #endif
	D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_1};
	if (D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, 
                        featureLevels, arrayCount(featureLevels), D3D11_SDK_VERSION, 
                        &device, &featureLevel, &deviceContext) < 0)
	{
    return false;
	}

  // SWAPCHAIN
  DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
  RECT clientAreaRect;
  GetClientRect(window, &clientAreaRect);
	swapChainDesc.Width = clientAreaRect.right;
	swapChainDesc.Height = clientAreaRect.bottom;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = multisamplingCount;
  UINT multisamplingQualityLevelsCount;
  device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, multisamplingCount, &multisamplingQualityLevelsCount);
  darAssert(multisamplingQualityLevelsCount != 0);
	swapChainDesc.SampleDesc.Quality = multisamplingQualityLevelsCount - 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;	// 2 back buffers + 1 front
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
	if (device->QueryInterface(__uuidof(IDXGIDevice), (void**)(dxgiDevice.GetAddressOf())) < 0) return false;
	Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
	if (dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)(dxgiAdapter.GetAddressOf())) < 0) return false;
	Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
	if (dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)(dxgiFactory.GetAddressOf())) < 0) return false;
	if (dxgiFactory->CreateSwapChainForHwnd(dxgiDevice.Get(), window, &swapChainDesc, NULL, NULL, &swapchain) < 0) return false;

  return true;
}