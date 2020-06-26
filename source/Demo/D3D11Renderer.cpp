#define DAR_MODULE_NAME "Renderer"
#include "D3D11Renderer.hpp"
#include "DarEngine.hpp"
#include "DarMath.hpp"

#include <d3d11_4.h>
#include <dwrite_2.h>
#include <d2d1_2.h>
#include <atlbase.h>

namespace
{
  ID3D11Device* device = nullptr;
  ID3D11DeviceContext* context = nullptr;
  D3D_FEATURE_LEVEL featureLevel = (D3D_FEATURE_LEVEL)0;
  IDXGISwapChain1* swapchain = nullptr;
  ID3D11RenderTargetView* renderTargetView = nullptr;
  ID3D11DepthStencilView* depthStencilView = nullptr;
  float clearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
  IDWriteFactory2* dwriteFactory;
  CComPtr<ID2D1Device1> d2Device = nullptr;
  CComPtr<ID2D1DeviceContext1> d2Context = nullptr;

  #ifdef DAR_DEBUG
    ID3D11RasterizerState* wireframeRasterizerState = nullptr;
    bool useWireframe = false;
    wchar_t debugTextString[4096];
    int debugTextStringLength = 0;
    CComPtr<ID2D1SolidColorBrush> debugTextBrush = nullptr;
    CComPtr<IDWriteTextFormat> debugTextFormat = nullptr;

    void _debugStringImpl(const wchar_t* newStr, int newStrLength)
    {
      int newDebugTextStringLength = debugTextStringLength + newStrLength;
      newDebugTextStringLength = clamp(newDebugTextStringLength, 0, (int)arrayCount(debugTextString));
      wchar_t* debugTextStringOffset = debugTextString + debugTextStringLength;
      int remainingDebugTextStringSpace =  arrayCount(debugTextString) - debugTextStringLength;
      if(remainingDebugTextStringSpace > 0)
      {
        _snwprintf_s(debugTextStringOffset, arrayCount(debugTextString) - debugTextStringLength, _TRUNCATE, L"%s\n", newStr);
        debugTextStringLength = newDebugTextStringLength + 1;
      }
    }
    #define debugString(...) \
    { \
      wchar_t newStr[256]; \
      int newStrLength = _snwprintf_s(newStr, _TRUNCATE, __VA_ARGS__); \
      if(newStrLength > 0) _debugStringImpl(newStr, newStrLength); \
    }
  #endif

  //TEMPORARY STUFF
  ID3D11Buffer* triangleVertexBuffer = nullptr;
  ID3D11VertexShader* triangleVertexShader = nullptr;
  ID3D11PixelShader* trianglePixelShader = nullptr;
  ID3D11InputLayout* triangleInputLayout = nullptr;
  ID3D11Buffer* triangleConstantBuffer = nullptr;

  void* loadShaderFile(const char* fileName, SIZE_T* shaderSize)
  {
    HANDLE file = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    LARGE_INTEGER fileSize;
    GetFileSizeEx(file, &fileSize);
    static byte shaderLoadBuffer[64 * 1024];
    DWORD bytesRead;
    ReadFile(file, shaderLoadBuffer, (DWORD)fileSize.QuadPart, &bytesRead, nullptr);
    *shaderSize = bytesRead;
    CloseHandle(file);
    return shaderLoadBuffer;
  }
  ID3D11VertexShader* loadVertexShader(const char* shaderName, D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT inputElementDescCount, ID3D11InputLayout** inputLayout)
  {
    char shaderFileName[128];
    _snprintf_s(shaderFileName, sizeof(shaderFileName), "%s.vs.cso", shaderName);
    SIZE_T shaderCodeSize;
    void* shaderCode = loadShaderFile(shaderFileName, &shaderCodeSize);
    ID3D11VertexShader* vertexShader;
    device->CreateVertexShader(shaderCode, shaderCodeSize, nullptr, &vertexShader);
    device->CreateInputLayout(inputElementDescs, inputElementDescCount, shaderCode, shaderCodeSize, inputLayout);
    return vertexShader;
  }
  ID3D11PixelShader* loadPixelShader(const char* name)
  {
    char shaderFileName[128];
    _snprintf_s(shaderFileName, sizeof(shaderFileName), "%s.ps.cso", name);
    SIZE_T shaderCodeSize;
    void* shaderCode = loadShaderFile(shaderFileName, &shaderCodeSize);
    ID3D11PixelShader* vertexShader;
    device->CreatePixelShader(shaderCode, shaderCodeSize, nullptr, &vertexShader);
    return vertexShader;
  }
}

bool initD3D11Renderer(HWND window)
{
  // DEVICE
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
  #ifdef DAR_DEBUG
	  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
  #endif
	D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_1};
	if (D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, 
                        featureLevels, arrayCount(featureLevels), D3D11_SDK_VERSION, 
                        &device, &featureLevel, &context) < 0)
	{
    return false;
	}
  // SWAPCHAIN
  DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
  RECT clientAreaRect;
  GetClientRect(window, &clientAreaRect);
  const UINT clientAreaWidth = clientAreaRect.right - clientAreaRect.left;
  const UINT clientAreaHeight = clientAreaRect.bottom - clientAreaRect.top;
	swapChainDesc.Width = clientAreaWidth;
	swapChainDesc.Height = clientAreaHeight;
	swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  const UINT multisamplingCount = 8;
	swapChainDesc.SampleDesc.Count = multisamplingCount;
  UINT multisamplingQualityLevelsCount;
  device->CheckMultisampleQualityLevels(swapChainDesc.Format, multisamplingCount, &multisamplingQualityLevelsCount);
  darAssert(multisamplingQualityLevelsCount != 0);
  const UINT multisamplingQuality = multisamplingQualityLevelsCount - 1;
	swapChainDesc.SampleDesc.Quality = multisamplingQuality;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;	// 1 back buffer + 1 front
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	CComPtr<IDXGIDevice> dxgiDevice;
	if(device->QueryInterface(__uuidof(IDXGIDevice), (void**)(&dxgiDevice)) >= 0)
  {
    CComPtr<IDXGIAdapter> dxgiAdapter;
	  if(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)(&dxgiAdapter)) >= 0) 
    {
      CComPtr<IDXGIFactory2> dxgiFactory;
	    if (dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)(&dxgiFactory)) >= 0)
      {
        dxgiFactory->CreateSwapChainForHwnd(dxgiDevice, window, &swapChainDesc, NULL, NULL, &swapchain);
        if(!swapchain)
        {
          logError("Failed to create swapchain");
          return false;
        }
      }
      else
      {
        logError("Failed to get IDXGIFactory");
        return false;
      }
    }
    else 
    {
      logError("Failed to get IDXGIAdapter");
      return false;
    }

    // DirectWrite and Direct2D
    if(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory2), (IUnknown**)&dwriteFactory) >= 0)
    {
      D2D1_FACTORY_OPTIONS options;
      #ifdef DAR_DEBUG
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
      #else
        options.debugLevel = D2D1_DEBUG_LEVEL_NONE;
      #endif
      CComPtr<ID2D1Factory2> d2dFactory;
      if(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory2),  &options, (void**)&d2dFactory) >= 0)
      {
        if(FAILED(d2dFactory->CreateDevice(dxgiDevice, &d2Device)))
        {
          logError("Failed to create ID2D1Device");
          return false;
        }
        if(FAILED(d2Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2Context)))
        {
          logError("Failed to create ID2D1DeviceContext");
          return false;
        }
      }
    }
  }
  else
  {
    logError("Failed to get IDXGIDevice.");
    return false;
  }

  // RENDER TARGET VIEW
  CComPtr<ID3D11Texture2D> backBuffer;
	if (swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&backBuffer)) >= 0)
	{
    device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
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

  context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

  // VIEWPORT
  D3D11_VIEWPORT viewport;
  viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = (float)clientAreaWidth;
	viewport.Height = (float)clientAreaHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);

  // TRIANGLE
  Vec3f triangleVertices[] = {
    { 0.0f,  0.5f, 0.0f}, { 1.0f, 0.0f, 0.0f}, 
    { 0.5f, -0.5f, 0.0f}, { 0.0f, 1.0f, 0.0f}, 
    {-0.5f, -0.5f, 0.0f}, { 0.0f, 0.0f, 1.0f}
  };
  D3D11_BUFFER_DESC triangleVBDesc
  {
    sizeof(triangleVertices),
    D3D11_USAGE_DEFAULT,
    D3D11_BIND_VERTEX_BUFFER
  };
  D3D11_SUBRESOURCE_DATA triangleVBData {triangleVertices, 0, 0};
  device->CreateBuffer(&triangleVBDesc, &triangleVBData, &triangleVertexBuffer);
  D3D11_INPUT_ELEMENT_DESC triangleInputElementDescs[] = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0}, 
    {"COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
  };
  triangleVertexShader = loadVertexShader("Triangle", triangleInputElementDescs, arrayCount(triangleInputElementDescs), &triangleInputLayout);
  trianglePixelShader = loadPixelShader("Triangle");
  D3D11_BUFFER_DESC triangleCBDesc
  {
    sizeof(Mat4f), 
    D3D11_USAGE_DEFAULT,
    D3D11_BIND_CONSTANT_BUFFER
  };
  device->CreateBuffer(&triangleCBDesc, nullptr, &triangleConstantBuffer);

  D2D1_BITMAP_PROPERTIES1 d2BitmapProperties{};
  d2BitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
  d2BitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
  d2BitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
  CComPtr<IDXGISurface> dxgiBackBuffer;
  if(FAILED(swapchain->GetBuffer(0, __uuidof(IDXGISurface), (void**)&dxgiBackBuffer)))
  {
    logError("Failed to get swapchains IDXGISurface back buffer");
    return false;
  }
  CComPtr<ID2D1Bitmap1> d2Bitmap;
  if(FAILED(d2Context->CreateBitmapFromDxgiSurface(dxgiBackBuffer, &d2BitmapProperties, &d2Bitmap)))
  {
    logError("Failed to create ID2D1Bitmap render target");
    return false;
  }
  d2Context->SetTarget(d2Bitmap);

  #ifdef DAR_DEBUG
    // WIREFRAME
    D3D11_RASTERIZER_DESC wireframeRasterizerDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
    wireframeRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
    device->CreateRasterizerState(&wireframeRasterizerDesc, &wireframeRasterizerState);

    // DEBUG TEXT
    d2Context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &debugTextBrush);
    dwriteFactory->CreateTextFormat(L"Arial", nullptr, DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12, L"en-US", &debugTextFormat);
    debugTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    debugTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
  #endif

  return true;
}

void render(const GameState& game)
{
  d2Context->BeginDraw();

  #ifdef DAR_DEBUG
    // WIREFRAME
    if(game.input.F1.pressedDown) 
    {
      useWireframe = !useWireframe;
      if(useWireframe) context->RSSetState(wireframeRasterizerState);
      else context->RSSetState(nullptr);
    }
    // DEBUG TEXT
    debugTextStringLength = 0;
    debugString(L"%.3fms / %dfps", game.dTime, (int)(1/game.dTime));
  #endif

  context->ClearRenderTargetView(renderTargetView, clearColor);
  context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  Mat4f transformation = Mat4f::translation(0.5f, 0.5f, 0.f);
  context->UpdateSubresource(triangleConstantBuffer, 0, nullptr, &transformation, 0, 0);
  context->VSSetShader(triangleVertexShader, nullptr, 0);
  context->VSSetConstantBuffers(0, 1, &triangleConstantBuffer);
  context->PSSetShader(trianglePixelShader, nullptr, 0);
  context->IASetInputLayout(triangleInputLayout);
  const UINT triangleStride = 2 * sizeof(Vec3f);
  const UINT triangleOffset = 0;
  context->IASetVertexBuffers(0, 1, &triangleVertexBuffer, &triangleStride, &triangleOffset);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  context->Draw(3, 0);

  #ifdef DAR_DEBUG
    // DEBUG TEXT
    CComPtr<IDWriteTextLayout> debugTextLayout;
    dwriteFactory->CreateTextLayout(debugTextString, debugTextStringLength, debugTextFormat, (float)game.clientAreaWidth, (float)game.clientAreaHeight, &debugTextLayout);
    d2Context->DrawTextLayout({5.f, 5.f}, debugTextLayout, debugTextBrush);
  #endif

  d2Context->EndDraw();
  UINT presentFlags = 0;
  swapchain->Present(1, presentFlags);
  context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
}
