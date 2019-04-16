#define DAR_MODULE_NAME "Renderer"
#include "D3D11Renderer.h"
#include "DarEngine.h"
#include <Windows.h>
#include <d3d11_4.h>

namespace
{
  ID3D11Device* device = nullptr;
  ID3D11DeviceContext* deviceContext = nullptr;
  D3D_FEATURE_LEVEL featureLevel = (D3D_FEATURE_LEVEL)0;
  IDXGISwapChain1* swapchain = nullptr;
  ID3D11RenderTargetView* renderTargetView = nullptr;
  ID3D11DepthStencilView* depthStencilView = nullptr;
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
  const UINT clientAreaWidth = clientAreaRect.right;
  const UINT clientAreaHeight = clientAreaRect.bottom;
	swapChainDesc.Width = clientAreaWidth;
	swapChainDesc.Height = clientAreaHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  const UINT multisamplingCount = 8;
	swapChainDesc.SampleDesc.Count = multisamplingCount;
  UINT multisamplingQualityLevelsCount;
  device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, multisamplingCount, &multisamplingQualityLevelsCount);
  darAssert(multisamplingQualityLevelsCount != 0);
  const UINT multisamplingQuality = multisamplingQualityLevelsCount - 1;
	swapChainDesc.SampleDesc.Quality = multisamplingQuality;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;	// 2 back buffers + 1 front
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	IDXGIDevice* dxgiDevice;
	if(device->QueryInterface(__uuidof(IDXGIDevice), (void**)(&dxgiDevice)) >= 0)
  {
    IDXGIAdapter* dxgiAdapter;
	  if(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)(&dxgiAdapter)) >= 0) 
    {
      IDXGIFactory2* dxgiFactory;
	    if (dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)(&dxgiFactory)) >= 0)
      {
        dxgiFactory->CreateSwapChainForHwnd(dxgiDevice, window, &swapChainDesc, NULL, NULL, &swapchain);

        dxgiFactory->Release();
      }
      dxgiAdapter->Release();
    }
    dxgiDevice->Release();
  } 
  if(!swapchain)
  {
    logError("Failed to initialize swapchain");
    return false;
  }

  // RENDER TARGET VIEW
  ID3D11Texture2D* backBuffer;
	if (swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&backBuffer)) >= 0)
	{
    device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
		backBuffer->Release();
	}
  if(!renderTargetView)
  {
    logError("Failed to create render target view");
    return false;
  }
    
  // DEPTH STENCIL VIEW
  D3D11_TEXTURE2D_DESC depthStencilDesc{};
	depthStencilDesc.Width = clientAreaWidth;
	depthStencilDesc.Height = clientAreaHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.SampleDesc.Count = multisamplingCount;
	depthStencilDesc.SampleDesc.Quality = multisamplingQuality;
  ID3D11Texture2D* depthStencilBuffer;
	if (device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer) < 0) 
  {
    logError("Failed to create depth stencil buffer");
    return false;
  }
	if (device->CreateDepthStencilView(depthStencilBuffer, nullptr, &depthStencilView) < 0)
	{
    logError("Failed to create depth stencil view");
	  return false;
  }

  // VIEWPORT
  D3D11_VIEWPORT viewport;
  viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = (float)clientAreaWidth;
	viewport.Height = (float)clientAreaHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	deviceContext->RSSetViewports(1, &viewport);

  return true;
}