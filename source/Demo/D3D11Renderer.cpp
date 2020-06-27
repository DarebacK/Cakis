#define DAR_MODULE_NAME "Renderer"
#include "D3D11Renderer.hpp"
#include "DarEngine.hpp"
#include "DarMath.hpp"

#include <d3d11_4.h>
#include <dwrite_2.h>
#include <d2d1_2.h>
#include <atlbase.h>

namespace Renderer
{
CComPtr<ID3D11Device> device = nullptr;
CComPtr<ID3D11DeviceContext> context = nullptr;
D3D_FEATURE_LEVEL featureLevel = (D3D_FEATURE_LEVEL)0;
CComPtr<IDXGISwapChain1> swapChain = nullptr;
DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
CComPtr<ID3D11RenderTargetView> renderTargetView = nullptr;
CComPtr<ID3D11DepthStencilView> depthStencilView = nullptr;
float clearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
CComPtr<IDWriteFactory2> dwriteFactory;
CComPtr<ID2D1Device1> d2Device = nullptr;
CComPtr<ID2D1DeviceContext1> d2Context = nullptr;

#ifdef DAR_DEBUG
  CComPtr<ID3D11Debug> debug = nullptr;
  CComPtr<ID3D11RasterizerState> wireframeRasterizerState = nullptr;
  bool useWireframe = false;
  CComPtr<ID2D1SolidColorBrush> debugTextBrush = nullptr;
  CComPtr<IDWriteTextFormat> debugTextFormat = nullptr;
#endif

//TEMPORARY STUFF
CComPtr<ID3D11Buffer> triangleVertexBuffer = nullptr;
CComPtr<ID3D11VertexShader> triangleVertexShader = nullptr;
CComPtr<ID3D11PixelShader> trianglePixelShader = nullptr;
CComPtr<ID3D11InputLayout> triangleInputLayout = nullptr;
CComPtr<ID3D11Buffer> triangleConstantBuffer = nullptr;

static void* loadShaderFile(const char* fileName, SIZE_T* shaderSize)
{
  HANDLE file = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
  if(file == INVALID_HANDLE_VALUE) {
    logError("Failed to load shader file %s", fileName);
    return nullptr;
  }
  LARGE_INTEGER fileSize;
  if(!GetFileSizeEx(file, &fileSize)) {
    logError("Failed to get file size of shader file %s", fileName);
    CloseHandle(file);
    return nullptr;
  }
  static byte shaderLoadBuffer[64 * 1024];
  DWORD bytesRead;
  if(!ReadFile(file, shaderLoadBuffer, (DWORD)fileSize.QuadPart, &bytesRead, nullptr)) {
    logError("Failed to read shader file %s", fileName);
    CloseHandle(file);
    return nullptr;
  }
  *shaderSize = bytesRead;
  CloseHandle(file);
  return shaderLoadBuffer;
}

static ID3D11VertexShader* loadVertexShader(const char* shaderName, D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT inputElementDescCount, ID3D11InputLayout** inputLayout)
{
  char shaderFileName[128];
  _snprintf_s(shaderFileName, sizeof(shaderFileName), "%s.vs.cso", shaderName);
  SIZE_T shaderCodeSize;
  void* shaderCode = loadShaderFile(shaderFileName, &shaderCodeSize);
  if(!shaderCode) {
    return nullptr;
  }
  ID3D11VertexShader* vertexShader;
  if(SUCCEEDED(device->CreateVertexShader(shaderCode, shaderCodeSize, nullptr, &vertexShader))) {
    if(SUCCEEDED(device->CreateInputLayout(inputElementDescs, inputElementDescCount, shaderCode, shaderCodeSize, inputLayout))) {
      return vertexShader;
    }
  } else {
    logError("Failed to create vertex shader %s", shaderName);
    return nullptr;
  }
  return nullptr;
}

static ID3D11PixelShader* loadPixelShader(const char* name)
{
  char shaderFileName[128];
  _snprintf_s(shaderFileName, sizeof(shaderFileName), "%s.ps.cso", name);
  SIZE_T shaderCodeSize;
  void* shaderCode = loadShaderFile(shaderFileName, &shaderCodeSize);
  ID3D11PixelShader* vertexShader;
  device->CreatePixelShader(shaderCode, shaderCodeSize, nullptr, &vertexShader);
  return vertexShader;
}

static void setViewport(FLOAT width, FLOAT height)
{
  D3D11_VIEWPORT viewport;
  viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);
}

static void updateViewport()
{
  setViewport((FLOAT)swapChainDesc.Width, (FLOAT)swapChainDesc.Height);
}

static CComPtr<ID3D11RenderTargetView> createRenderTargetView()
{
  CComPtr<ID3D11Texture2D> backBuffer;
	if(FAILED(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)))) {
    logError("Failed to get back buffer for render target view.");
    return nullptr;
	}
  CComPtr<ID3D11RenderTargetView> result = nullptr;
  if(FAILED(device->CreateRenderTargetView(backBuffer, nullptr, &result))) {
    logError("Failed to create render target view");
    return nullptr;
  }
  return result;
}

static CComPtr<ID3D11DepthStencilView> createDepthStencilView()
{
  D3D11_TEXTURE2D_DESC depthStencilDesc{};
	depthStencilDesc.Width = swapChainDesc.Width;
	depthStencilDesc.Height = swapChainDesc.Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.SampleDesc.Count = swapChainDesc.SampleDesc.Count;
	depthStencilDesc.SampleDesc.Quality = swapChainDesc.SampleDesc.Quality;
  CComPtr<ID3D11Texture2D> depthStencilBuffer = nullptr;
	if(FAILED(device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer))) {
    logError("Failed to create depth stencil buffer");
    return nullptr;
  }

  CComPtr<ID3D11DepthStencilView> result = nullptr;
	if(FAILED(device->CreateDepthStencilView(depthStencilBuffer, nullptr, &result))) {
    logError("Failed to create depth stencil view");
	  return nullptr;
  }
  return result;
}

static bool bindD2dTargetToD3dTarget()
{
  CComPtr<IDXGISurface> dxgiBackBuffer;
  if(FAILED(swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer))))
  {
    logError("Failed to get swapchains IDXGISurface back buffer");
    return false;
  }
  D2D1_BITMAP_PROPERTIES1 d2BitmapProperties{};
  d2BitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
  d2BitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
  d2BitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
  CComPtr<ID2D1Bitmap1> d2Bitmap;
  if(FAILED(d2Context->CreateBitmapFromDxgiSurface(
    dxgiBackBuffer, 
    &d2BitmapProperties, 
    &d2Bitmap
  ))) {
    logError("Failed to create ID2D1Bitmap render target");
    return false;
  }
  d2Context->SetTarget(d2Bitmap);
  return true;
}

bool init(HWND window)
{
  // DEVICE
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
  #ifdef DAR_DEBUG
	  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
  #endif
	D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_1};
	if(FAILED(D3D11CreateDevice(
    NULL, 
    D3D_DRIVER_TYPE_HARDWARE,
    NULL, 
    createDeviceFlags, 
    featureLevels, 
    arrayCount(featureLevels), 
    D3D11_SDK_VERSION, 
    &device, 
    &featureLevel, 
    &context
  ))) {
    logError("Failed to create D3D11 device");
    return false;
	}

  #ifdef DAR_DEBUG
    device->QueryInterface(IID_PPV_ARGS(&debug));
  #endif

  // SWAPCHAIN
	swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 8;
  UINT multisamplingQualityLevelsCount;
  device->CheckMultisampleQualityLevels(swapChainDesc.Format, swapChainDesc.SampleDesc.Count, &multisamplingQualityLevelsCount);
  darAssert(multisamplingQualityLevelsCount != 0);
  const UINT multisamplingQuality = multisamplingQualityLevelsCount - 1;
	swapChainDesc.SampleDesc.Quality = multisamplingQuality;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;	// 1 back buffer + 1 front
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	CComPtr<IDXGIDevice> dxgiDevice;
	if(SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&dxgiDevice)))) {
    CComPtr<IDXGIAdapter> dxgiAdapter;
	  if(SUCCEEDED(dxgiDevice->GetParent(IID_PPV_ARGS(&dxgiAdapter)))) {
      CComPtr<IDXGIFactory2> dxgiFactory;
	    if (SUCCEEDED(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)))) {
        dxgiFactory->CreateSwapChainForHwnd(dxgiDevice, window, &swapChainDesc, NULL, NULL, &swapChain);
        if(!swapChain) {
          logError("Failed to create swapChain");
          return false;
        }
        swapChain->GetDesc1(&swapChainDesc);
      }
      else {
        logError("Failed to get IDXGIFactory");
        return false;
      }
    }
    else {
      logError("Failed to get IDXGIAdapter");
      return false;
    }

    // DirectWrite and Direct2D
    if(SUCCEEDED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory2), (IUnknown**)&dwriteFactory))) {
      D2D1_FACTORY_OPTIONS options;
      #ifdef DAR_DEBUG
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
      #else
        options.debugLevel = D2D1_DEBUG_LEVEL_NONE;
      #endif
      CComPtr<ID2D1Factory2> d2dFactory;
      if(SUCCEEDED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory2), &options, (void**)&d2dFactory))) {
        if(FAILED(d2dFactory->CreateDevice(dxgiDevice, &d2Device))) {
          logError("Failed to create ID2D1Device");
          return false;
        }
        if(FAILED(d2Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2Context))) {
          logError("Failed to create ID2D1DeviceContext");
          return false;
        }
      }
    }
  } else {
    logError("Failed to get IDXGIDevice.");
    return false;
  }

  renderTargetView = createRenderTargetView();
  if(!renderTargetView) {
    logError("Failed to initialize render target view.");
    return false;
  }
  depthStencilView = createDepthStencilView();
  if(!depthStencilView) {
    logError("Failed to initialize depth stencil view.");
    return false;
  }
  context->OMSetRenderTargets(1, &renderTargetView.p, depthStencilView);

  updateViewport();

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

  bindD2dTargetToD3dTarget();

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

void onWindowResize(int clientAreaWidth, int clientAreaHeight)
{
  if(swapChain) {
    d2Context->SetTarget(nullptr);  // clears the binding to swapChain's back buffer
    depthStencilView.Release();
    renderTargetView.Release();
    if(FAILED(swapChain->ResizeBuffers(0, 0, 0, swapChainDesc.Format, swapChainDesc.Flags))) {
      logError("Failed to resize swapChain buffers");
#ifdef DAR_DEBUG
      debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_SUMMARY);
#endif DAR_DEBUG
    }
    if(FAILED(swapChain->GetDesc1(&swapChainDesc))) {
      logError("Failed to get swapChain desc after window resize");
    }

    renderTargetView = createRenderTargetView();
    if(!renderTargetView) {
      logError("Failed to create render target view after window resize.");
    }
    depthStencilView = createDepthStencilView();
    if(!depthStencilView) {
      logError("Failed to create depth stencil view after window resize.");
    }
    context->OMSetRenderTargets(1, &renderTargetView.p, depthStencilView);

    if(!bindD2dTargetToD3dTarget()) {
      logError("Failed to bind Direct2D render target to Direct3D render target.");
    }

    updateViewport();
  }
}

static void switchWireframeState()
{
#ifdef DAR_DEBUG
  useWireframe = !useWireframe;
  if(useWireframe) context->RSSetState(wireframeRasterizerState);
  else context->RSSetState(nullptr);
#endif
}

void render(const GameState& game)
{
  d2Context->BeginDraw();

  #ifdef DAR_DEBUG
    if(game.input.F1.pressedDown) {
      switchWireframeState();
    }
    debugString(L"%.3fms / %dfps", game.dTime, (int)(1/game.dTime));
  #endif

  context->ClearRenderTargetView(renderTargetView, clearColor);
  context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  Mat4f transformation = Mat4f::translation(0.5f, 0.5f, 0.f);
  context->UpdateSubresource(triangleConstantBuffer, 0, nullptr, &transformation, 0, 0);
  context->VSSetShader(triangleVertexShader, nullptr, 0);
  context->VSSetConstantBuffers(0, 1, &triangleConstantBuffer.p);
  context->PSSetShader(trianglePixelShader, nullptr, 0);
  context->IASetInputLayout(triangleInputLayout);
  constexpr UINT triangleStride = 2 * sizeof(Vec3f);
  constexpr UINT triangleOffset = 0;
  context->IASetVertexBuffers(0, 1, &triangleVertexBuffer.p, &triangleStride, &triangleOffset);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  context->Draw(3, 0);

  #ifdef DAR_DEBUG
    // DEBUG TEXT
    CComPtr<IDWriteTextLayout> debugTextLayout;
    dwriteFactory->CreateTextLayout(
      _debugTextString, 
      _debugTextStringLength, 
      debugTextFormat, 
      (float)game.clientAreaWidth, 
      (float)game.clientAreaHeight, 
      &debugTextLayout
    );
    d2Context->DrawTextLayout({5.f, 5.f}, debugTextLayout, debugTextBrush);
  #endif

  d2Context->EndDraw();
  UINT presentFlags = 0;
  swapChain->Present(1, presentFlags);
}
} // namespace Renderer
