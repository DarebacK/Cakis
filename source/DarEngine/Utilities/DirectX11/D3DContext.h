#pragma once

// Forward declaration
namespace DE {namespace Utilities {namespace Win32 {
	class Window;
}
}
}

namespace DE
{
namespace Utilities
{
namespace DirectX11
{
	class D3DContext
	{
	public:
		explicit	D3DContext(const Win32::Window& window);
					D3DContext(const D3DContext& other) = delete;
		D3DContext&	operator=(const D3DContext& rhs) = delete;
					D3DContext(D3DContext&& other) = default;
		D3DContext&	operator=(D3DContext&& rhs) = default;
					~D3DContext();

	private:
		Microsoft::WRL::ComPtr<ID3D11Device>			m_d3dDevice{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>		m_d3dDeviceContext{ nullptr};
		D3D_FEATURE_LEVEL								m_d3dFeatureLevel{};
		UINT											m_d3dMultiSamplingCount{ 4 };
		UINT											m_d3dMultiSamplingQualityLevelCount{ 0 };
		bool											m_d3disMultiSamplingEnabled{ true };
		Microsoft::WRL::ComPtr<IDXGISwapChain1>			m_dxgiSwapChain{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	m_d3dRenderTargetView{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11Texture2D>			m_d3dDepthStencilBuffer{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	m_d3dDepthStencilView{ nullptr };
		D3D11_VIEWPORT									m_d3dViewport{};

		void											InitializeDevice();
		void											CheckMultiSamplingQualityLevels();
		void											InitializeSwapChain(const Win32::Window& window);
		void											InitializeRenderTargetView();
		void											InitializeDepthStencilView(const Win32::Window& window);
		void											BindViewsToOutputMerger();
		void											SetupViewPort(const Win32::Window& window);
	};
}
}
}
