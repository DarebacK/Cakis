#define DAR_MODULE_NAME "Renderer"

#include <vector>

#include <d3d11_4.h>
#include <dwrite_2.h>
#include <d2d1_2.h>
#include <atlbase.h>

#include "D3D11Renderer.hpp"
#include "DarEngine.hpp"
#include "DarMath.hpp"

namespace Renderer
{
CComPtr<ID3D11Device> device = nullptr;
CComPtr<ID3D11DeviceContext> context = nullptr;
D3D_FEATURE_LEVEL featureLevel = (D3D_FEATURE_LEVEL)0;
CComPtr<IDXGIAdapter3> dxgiAdapter;
DXGI_ADAPTER_DESC2 dxgiAdapterDesc{};
CComPtr<IDXGISwapChain1> swapChain = nullptr;
DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
constexpr UINT msaaSampleCount = 8;
CComPtr<ID3D11Texture2D> renderTarget = nullptr;
CD3D11_TEXTURE2D_DESC renderTargetDesc = {};
CComPtr<ID3D11RenderTargetView> renderTargetView = nullptr;
CComPtr<ID3D11DepthStencilView> depthStencilView = nullptr;
float clearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
CComPtr<ID3D11RasterizerState> rasterizerState = nullptr;
D3D11_RASTERIZER_DESC rasterizerDesc =
{
  D3D11_FILL_SOLID,
  D3D11_CULL_BACK,
  FALSE, // FrontCounterClockwise
  D3D11_DEFAULT_DEPTH_BIAS,
  D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
  D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
  TRUE, // DepthClipEnable
  FALSE, // ScissorEnable
  TRUE, // MultisampleEnable
  TRUE // AntialiasedLineEnable
};
CComPtr<IDWriteFactory2> dwriteFactory;
CComPtr<ID2D1Device1> d2Device = nullptr;
CComPtr<ID2D1DeviceContext1> d2Context = nullptr;
constexpr float verticalFieldOfView = 74.f;
constexpr float nearPlane = 1.f;
constexpr float farPlane = 100.f;
Mat4f projectionMatrix = Mat4f::identity();

#ifdef DAR_DEBUG
  CComPtr<ID3D11Debug> debug = nullptr;
  CComPtr<ID2D1SolidColorBrush> debugTextBrush = nullptr;
  CComPtr<IDWriteTextFormat> debugTextFormat = nullptr;
#endif

  static void* loadShaderFile(const char* fileName, SIZE_T* shaderSize);
  static ID3D11VertexShader* loadVertexShader(const char* shaderName, D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT inputElementDescCount, ID3D11InputLayout** inputLayout);
  static ID3D11PixelShader* loadPixelShader(const char* name);

  // Grid
  CComPtr<ID3D11VertexShader> gridVertexShader = nullptr;
  CComPtr<ID3D11PixelShader> gridPixelShader = nullptr;
  CComPtr<ID3D11InputLayout> gridInputLayout = nullptr;
  CComPtr<ID3D11Buffer> gridConstantBuffer = nullptr;
  struct Grid
  {
    Mat4f transformation;
    CComPtr<ID3D11Buffer> vertexBuffer;
    const int vertexCount;
  };
  static constexpr int calculateGridVertexCount(int x, int y) { return 2 * (x + 1 + y + 1); }
  Grid grids[5] = {
    {Mat4f::rotationX(degreesToRadians(90)), nullptr, calculateGridVertexCount(GameState::gridSize.x, GameState::gridSize.z)}, // Bottom.
    {Mat4f::rotationY(degreesToRadians(-90)), nullptr, calculateGridVertexCount(GameState::gridSize.z, GameState::gridSize.y)}, // Left.
    {toMat4f(Mat3f::rotationY(degreesToRadians(-90)) * Mat4x3f::translation((float)GameState::gridSize.x, 0.f, 0.f)), nullptr, calculateGridVertexCount(GameState::gridSize.z, GameState::gridSize.y)}, // Right.
    {Mat4f::identity(), nullptr, calculateGridVertexCount(GameState::gridSize.x, GameState::gridSize.y)}, // Front.
    {Mat4f::translation(0.f, 0.f, (float)GameState::gridSize.z), nullptr, calculateGridVertexCount(GameState::gridSize.x, GameState::gridSize.y)} // Back.
  };
  static void generateGridVertices(int xSize, int ySize, Vec2f* output)
  {
    int gridVertexIndex = 0;
    // Vertical lines
    for(int i = 0; i <= xSize; ++i) {
      output[gridVertexIndex].x = (float)i;
      output[gridVertexIndex].y = 0.f;
      ++gridVertexIndex;
      output[gridVertexIndex].x = (float)i;
      output[gridVertexIndex].y = (float)ySize;
      ++gridVertexIndex;
    }
    // Horizontal lines
    for(int i = 0; i <= ySize; ++i) {
      output[gridVertexIndex].x = 0.f;
      output[gridVertexIndex].y = (float)i;
      ++gridVertexIndex;
      output[gridVertexIndex].x = (float)xSize;
      output[gridVertexIndex].y = (float)i;
      ++gridVertexIndex;
    }
  }
  static void generateGrid(std::vector<Vec2f>& vertices, int xSize, int ySize, ID3D11Buffer** vertexBuffer)
  {
    generateGridVertices(xSize, ySize, vertices.data());
    D3D11_BUFFER_DESC vertexBufferDesc
    {
      UINT(vertices.size() * sizeof(Vec2f)),
      D3D11_USAGE_IMMUTABLE,
      D3D11_BIND_VERTEX_BUFFER
    };
    D3D11_SUBRESOURCE_DATA vertexBufferData{ vertices.data(), 0, 0 };
    if(FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, vertexBuffer))) {
      throw InitializeException("Failed to create grid vertex buffer.");
    }
  }
  static void initializeGrid()
  {
    std::vector<Vec2f> vertices(grids[0].vertexCount);
    generateGrid(vertices, GameState::gridSize.x, GameState::gridSize.z, &grids[0].vertexBuffer);
    if(GameState::gridSize.z == GameState::gridSize.x && GameState::gridSize.y == GameState::gridSize.z) {
      grids[1].vertexBuffer = grids[0].vertexBuffer;
      grids[2].vertexBuffer = grids[0].vertexBuffer;
    } else {
      vertices.resize(grids[1].vertexCount);
      generateGrid(vertices, GameState::gridSize.z, GameState::gridSize.y, &grids[1].vertexBuffer);
      grids[2].vertexBuffer = grids[1].vertexBuffer;
    }
    if(GameState::gridSize.y == GameState::gridSize.z) {
      grids[3].vertexBuffer = grids[0].vertexBuffer;
      grids[4].vertexBuffer = grids[0].vertexBuffer;
    } else if(GameState::gridSize.x == GameState::gridSize.z) {
      grids[3].vertexBuffer = grids[1].vertexBuffer;
      grids[4].vertexBuffer = grids[1].vertexBuffer;
    } else {
      vertices.resize(grids[3].vertexCount);
      generateGrid(vertices, GameState::gridSize.x, GameState::gridSize.y, &grids[3].vertexBuffer);
      grids[4].vertexBuffer = grids[3].vertexBuffer;
    }

    D3D11_INPUT_ELEMENT_DESC gridInputElementDescs[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    gridVertexShader = loadVertexShader("grid", gridInputElementDescs, arrayCount(gridInputElementDescs), &gridInputLayout);
    gridPixelShader = loadPixelShader("grid");
    D3D11_BUFFER_DESC gridCBDesc
    {
      sizeof(Mat4f),
      D3D11_USAGE_DYNAMIC,
      D3D11_BIND_CONSTANT_BUFFER,
      D3D11_CPU_ACCESS_WRITE
    };
    device->CreateBuffer(&gridCBDesc, nullptr, &gridConstantBuffer);
  }
  static void renderGrids(const Mat4f& viewProjection)
  {
    context->VSSetShader(gridVertexShader, nullptr, 0);
    context->PSSetShader(gridPixelShader, nullptr, 0);
    context->IASetInputLayout(gridInputLayout);
    context->VSSetConstantBuffers(0, 1, &gridConstantBuffer.p);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    constexpr UINT vertexBufferStride = sizeof(Vec2f);
    constexpr UINT vertexBufferOffset = 0;
    for(const Grid& grid : grids) {
      Mat4f transform = grid.transformation * viewProjection;
      context->Map(gridConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
      memcpy(mappedResource.pData, &transform, sizeof(transform));
      context->Unmap(gridConstantBuffer, 0);
      context->IASetVertexBuffers(0, 1, &grid.vertexBuffer.p, &vertexBufferStride, &vertexBufferOffset);
      context->Draw(grid.vertexCount, 0);
    }
  }

// Cube
CComPtr<ID3D11Buffer> cubeVertexBuffer = nullptr;
CComPtr<ID3D11Buffer> cubeInstanceBuffer = nullptr;
struct CubeInstanceData
{
  Mat4f transform;
  ColorRgbaf color;
} cubeInstanceData[GameState::gridSize.x * GameState::gridSize.y * GameState::gridSize.z + 4];
CComPtr<ID3D11Buffer> cubeIndexBuffer = nullptr;
CComPtr<ID3D11VertexShader> cubeVertexShader = nullptr;
CComPtr<ID3D11PixelShader> cubePixelShader = nullptr;
CComPtr<ID3D11InputLayout> cubeInputLayout = nullptr;

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
  setViewport((FLOAT)renderTargetDesc.Width, (FLOAT)renderTargetDesc.Height);

  float aspectRatio = (float)renderTargetDesc.Width / renderTargetDesc.Height;
  projectionMatrix = Mat4f::perspectiveProjectionD3d(
    degreesToRadians(verticalFieldOfView), 
    aspectRatio, 
    nearPlane, 
    farPlane
  );
}

static CComPtr<ID3D11Texture2D> createRenderTarget()
{
  renderTargetDesc = CD3D11_TEXTURE2D_DESC(
    swapChainDesc.Format,
    swapChainDesc.Width,
    swapChainDesc.Height,
    1,
    1,
    D3D11_BIND_RENDER_TARGET,
    D3D11_USAGE_DEFAULT,
    0,
    msaaSampleCount
  );

  CComPtr<ID3D11Texture2D> result = nullptr;
  if(FAILED((device->CreateTexture2D(&renderTargetDesc, nullptr, &result)))) {
    logError("Failed to create render target texture.");
    return nullptr;
  }
  return result;
}

static CComPtr<ID3D11RenderTargetView> createRenderTargetView(ID3D11Resource* renderTarget, DXGI_FORMAT format)
{
  CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2DMS, format);
  CComPtr<ID3D11RenderTargetView> result = nullptr;
  if(FAILED(device->CreateRenderTargetView(renderTarget, &renderTargetViewDesc, &result))) {
    logError("Failed to create render target view.");
    return nullptr;
  }
  return result;
}

static CComPtr<ID3D11DepthStencilView> createDepthStencilView()
{
  D3D11_TEXTURE2D_DESC depthStencilDesc{};
  depthStencilDesc.Width = renderTargetDesc.Width;
  depthStencilDesc.Height = renderTargetDesc.Height;
  depthStencilDesc.MipLevels = 1;
  depthStencilDesc.ArraySize = 1;
  depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
  depthStencilDesc.SampleDesc.Count = renderTargetDesc.SampleDesc.Count;
  depthStencilDesc.SampleDesc.Quality = renderTargetDesc.SampleDesc.Quality;
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
    logError("Failed to get swapchains IDXGISurface back buffer.");
    return false;
  }
  D2D1_BITMAP_PROPERTIES1 d2BitmapProperties{};
  d2BitmapProperties.pixelFormat.format = swapChainDesc.Format;
  d2BitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
  d2BitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
  CComPtr<ID2D1Bitmap1> d2Bitmap;
  if(FAILED(d2Context->CreateBitmapFromDxgiSurface(
    dxgiBackBuffer, 
    &d2BitmapProperties, 
    &d2Bitmap
  ))) {
    logError("Failed to create ID2D1Bitmap render target.");
    return false;
  }
  d2Context->SetTarget(d2Bitmap);
  return true;
}

void initialize(HWND window)
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
    throw InitializeException("Failed to create D3D11 device");
  }

  #ifdef DAR_DEBUG
    device->QueryInterface(IID_PPV_ARGS(&debug));
  #endif

  // SWAPCHAIN
  CComPtr<IDXGIDevice> dxgiDevice;
  if(SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&dxgiDevice)))) {
    if(SUCCEEDED(dxgiDevice->GetParent(IID_PPV_ARGS(&dxgiAdapter)))) {
      CComPtr<IDXGIFactory2> dxgiFactory;
      if(SUCCEEDED(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)))) {
        swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.Flags = 0;
        dxgiFactory->CreateSwapChainForHwnd(dxgiDevice, window, &swapChainDesc, NULL, NULL, &swapChain);
        if(!swapChain) {
          // DXGI_SWAP_EFFECT_FLIP_DISCARD may not be supported on this OS. Try legacy DXGI_SWAP_EFFECT_DISCARD.
          swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
          dxgiFactory->CreateSwapChainForHwnd(dxgiDevice, window, &swapChainDesc, NULL, NULL, &swapChain);
          if(!swapChain) {
            throw InitializeException("Failed to create swapChain.");
          }
        }
        swapChain->GetDesc1(&swapChainDesc);
        if(FAILED(dxgiFactory->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER))) {
          logError("Failed to make window association to ignore alt enter.");
        }
      } else {
        throw InitializeException("Failed to get IDXGIFactory");
      }

      if(FAILED(dxgiAdapter->GetDesc2(&dxgiAdapterDesc))) {
        logError("Failed to get adapter description.");
      }
    }
    else {
      throw InitializeException("Failed to get IDXGIAdapter.");
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
          throw InitializeException("Failed to create ID2D1Device");
        }
        if(FAILED(d2Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2Context))) {
          throw InitializeException("Failed to create ID2D1DeviceContext");
        }
      }
    }
  } else {
    throw InitializeException("Failed to get IDXGIDevice.");
  }

  renderTarget = createRenderTarget();
  if(!renderTarget) {
    throw InitializeException("Failed to create render target.");
  }

  renderTargetView = createRenderTargetView(renderTarget, renderTargetDesc.Format);
  if(!renderTargetView) {
    throw InitializeException("Failed to initialize render target view.");
  }
  depthStencilView = createDepthStencilView();
  if(!depthStencilView) {
    throw InitializeException("Failed to initialize depth stencil view.");
  }
  context->OMSetRenderTargets(1, &renderTargetView.p, depthStencilView);

  updateViewport();

  // Cube
  Vec3f cubeVertices[] = {
    { -0.5f, -0.5f, -0.5f },
    {  0.5f, -0.5f, -0.5f },
    {  0.5f, -0.5f,  0.5f },
    { -0.5f, -0.5f,  0.5f },
    { -0.5f,  0.5f, -0.5f },
    {  0.5f,  0.5f, -0.5f },
    {  0.5f,  0.5f,  0.5f },
    { -0.5f,  0.5f,  0.5f }
  };
  D3D11_BUFFER_DESC cubeVertexBufferDesc
  {
    sizeof(cubeVertices),
    D3D11_USAGE_IMMUTABLE,
    D3D11_BIND_VERTEX_BUFFER
  };
  D3D11_SUBRESOURCE_DATA cubeVBData {cubeVertices, 0, 0};
  if(FAILED(device->CreateBuffer(&cubeVertexBufferDesc, &cubeVBData, &cubeVertexBuffer))) {
    throw InitializeException("Failed to create cube vertex buffer.");
  }
  D3D11_BUFFER_DESC cubeInstanceBufferDesc
  {
    sizeof(cubeInstanceData),
    D3D11_USAGE_DYNAMIC,
    D3D11_BIND_VERTEX_BUFFER,
    D3D11_CPU_ACCESS_WRITE
  };
  if(FAILED(device->CreateBuffer(&cubeInstanceBufferDesc, nullptr, &cubeInstanceBuffer))) {
    throw InitializeException("Failed to create cube instance buffer.");
  }
  short cubeIndices[] = { 
    0,4,5, 0,5,1, // front
    1,5,6, 1,6,2, // right
    2,6,7, 2,7,3, // back
    3,7,4, 3,4,0, // left
    4,7,6, 4,6,5, // top
    3,0,1, 3,1,2  // bottom
  };
  D3D11_BUFFER_DESC cubeIBDesc
  {
    sizeof(cubeIndices),
    D3D11_USAGE_IMMUTABLE,
    D3D11_BIND_INDEX_BUFFER
  };
  D3D11_SUBRESOURCE_DATA cubeIBData{cubeIndices, 0, 0};
  if(FAILED(device->CreateBuffer(&cubeIBDesc, &cubeIBData, &cubeIndexBuffer))) {
    throw InitializeException("Failed to create cube index buffer.");
  }
  D3D11_INPUT_ELEMENT_DESC cubeInputElementDescs[] = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"INSTANCE_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
    {"INSTANCE_TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
    {"INSTANCE_TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
    {"INSTANCE_TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
    {"INSTANCE_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1}
  };
  cubeVertexShader = loadVertexShader("cube", cubeInputElementDescs, arrayCount(cubeInputElementDescs), &cubeInputLayout);
  cubePixelShader = loadPixelShader("cube");

  initializeGrid();

  device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
  context->RSSetState(rasterizerState);

  bindD2dTargetToD3dTarget();

  #ifdef DAR_DEBUG
    // DEBUG TEXT
    d2Context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &debugTextBrush);
    dwriteFactory->CreateTextFormat(L"Arial", nullptr, DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12, L"en-US", &debugTextFormat);
    debugTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    debugTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
  #endif
}

void onWindowResize(int clientAreaWidth, int clientAreaHeight)
{
  if(swapChain) {
    d2Context->SetTarget(nullptr);  // Clears the binding to swapChain's back buffer.
    depthStencilView.Release();
    renderTargetView.Release();
    renderTarget.Release();
    if(FAILED(swapChain->ResizeBuffers(0, 0, 0, swapChainDesc.Format, swapChainDesc.Flags))) {
      #ifdef DAR_DEBUG
        debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_SUMMARY);
      #endif DAR_DEBUG
      throw Exception("Failed to resize swapChain buffers");
    }
    if(FAILED(swapChain->GetDesc1(&swapChainDesc))) {
      throw Exception("Failed to get swapChain desc after window resize");
    }

    renderTarget = createRenderTarget();
    if(!renderTarget) {
      throw Exception("Failed to create render target after window resize.");
    }

    renderTargetView = createRenderTargetView(renderTarget, renderTargetDesc.Format);
    if(!renderTargetView) {
      throw Exception("Failed to create render target view after window resize.");
    }
    depthStencilView = createDepthStencilView();
    if(!depthStencilView) {
      throw Exception("Failed to create depth stencil view after window resize.");
    }
    context->OMSetRenderTargets(1, &renderTargetView.p, depthStencilView);

    if(!bindD2dTargetToD3dTarget()) {
      throw Exception("Failed to bind Direct2D render target to Direct3D render target after window resize.");
    }

    updateViewport();
  }
}

static void switchWireframeState()
{
#ifdef DAR_DEBUG
  rasterizerDesc.FillMode = rasterizerDesc.FillMode == D3D11_FILL_SOLID ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
  rasterizerState.Release();
  device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
  context->RSSetState(rasterizerState);
#endif
}

static void resolveRenderTargetIntoBackBuffer()
{
  CComPtr<ID3D11Texture2D> backBuffer = nullptr;
  if(SUCCEEDED(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)))) {
    context->ResolveSubresource(backBuffer, 0, renderTarget, 0, swapChainDesc.Format);
  } else {
    logError("resolveRenderTargetIntoBackBuffer failed. Failed to get swapchain's back buffer.");
  }
}

static void renderCubes(
  const PlayingSpace& playingSpace, 
  const Mat4f& viewProjection, 
  const CubeClass* cubeClasses,
  const Tetracube& currentTetracube
)
{
  constexpr Vec3f cubePositionOffset = { 0.5f, 0.5f, 0.5f };
  Mat4f baseTransform = Mat4x3f::translation(cubePositionOffset) * viewProjection;

  context->VSSetShader(cubeVertexShader, nullptr, 0);
  context->PSSetShader(cubePixelShader, nullptr, 0);
  context->IASetInputLayout(cubeInputLayout);
  ID3D11Buffer* const vertexBuffers[] = {cubeVertexBuffer.p, cubeInstanceBuffer.p};
  constexpr UINT cubeVertexBufferStrides[] = { sizeof(Vec3f), sizeof(CubeInstanceData) };
  constexpr UINT cubeVertexBufferOffsets[] = {0, 0};
  context->IASetVertexBuffers(0, arrayCount(vertexBuffers), vertexBuffers, cubeVertexBufferStrides, cubeVertexBufferOffsets);
  context->IASetIndexBuffer(cubeIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  D3D11_MAPPED_SUBRESOURCE mappedResource;

  Vec3i size = playingSpace.getSize();
  int instanceCount = 0;
  for(int y = 0; y < size.y; ++y) {
    for(int z = 0; z < size.z; ++z) {
      for(int x = 0; x < size.x; ++x) {
        PlayingSpace::ValueType cubeClassIndex = playingSpace.at(x, y, z);
        if(cubeClassIndex >= 0) {
          cubeInstanceData[instanceCount].transform = Mat4f::translation((float)x, (float)y, (float)z) * baseTransform;
          cubeInstanceData[instanceCount].color = cubeClasses[cubeClassIndex].color;
          ++instanceCount;
        }
      }
    }
  }

  for(const Vec3i& position : currentTetracube.positions) {
    cubeInstanceData[instanceCount].transform = Mat4x3f::translation(toVec3f(position + currentTetracube.translation)) * baseTransform;
    cubeInstanceData[instanceCount].color = cubeClasses[currentTetracube.cubeClassIndex].color;
    ++instanceCount;
  }

  context->Map(cubeInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  memcpy(mappedResource.pData, &cubeInstanceData, sizeof(cubeInstanceData));
  context->Unmap(cubeInstanceBuffer, 0);

  context->DrawIndexedInstanced(36, instanceCount, 0, 0, 0);
}

void render(const GameState& gameState)
{
  d2Context->BeginDraw();

  #ifdef DAR_DEBUG
    if(gameState.input.keyboard.F1.pressedDown) {
      switchWireframeState();
    }

    DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo{};
    UINT64 videoMemoryUsageMB = 0;
    if(SUCCEEDED(dxgiAdapter->QueryVideoMemoryInfo(
      0, 
      DXGI_MEMORY_SEGMENT_GROUP_LOCAL, 
      &videoMemoryInfo
    ))) {
      videoMemoryUsageMB = videoMemoryInfo.CurrentUsage / (1ull << 20ull); 
    } else {
      logError("Failed to query video memory info.");
    }
    UINT64 videoMemoryTotalMB = dxgiAdapterDesc.DedicatedVideoMemory / (1ull << 20ull);
    debugText(L"VRAM %llu MB / %llu MB", videoMemoryUsageMB, videoMemoryTotalMB); 
  #endif

  context->ClearRenderTargetView(renderTargetView, clearColor);
  context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  Mat4x3f viewMatrix = gameState.camera.calculateView({GameState::gridSize.x / 2.f, GameState::gridSize.y / 2.f, GameState::gridSize.z / 2.f });
  Mat4f viewProjection = viewMatrix * projectionMatrix;

  renderCubes(gameState.playingSpace, viewProjection, gameState.cubeClasses, gameState.currentTetracube);

  renderGrids(viewProjection);

  resolveRenderTargetIntoBackBuffer();

  #ifdef DAR_DEBUG
    // DEBUG TEXT
    CComPtr<IDWriteTextLayout> debugTextLayout;
    dwriteFactory->CreateTextLayout(
      _debugText, 
      _debugTextLength, 
      debugTextFormat, 
      (FLOAT)gameState.clientAreaWidth, 
      (FLOAT)gameState.clientAreaHeight, 
      &debugTextLayout
    );
    d2Context->DrawTextLayout({5.f, 5.f}, debugTextLayout, debugTextBrush);
  #endif

  d2Context->EndDraw();
  UINT presentFlags = 0;
  swapChain->Present(1, presentFlags);
}
} // namespace Renderer
