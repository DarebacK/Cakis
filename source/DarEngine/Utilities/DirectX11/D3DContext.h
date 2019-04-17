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
					D3DContext(const HWND windowHandle, int clientAreaWidth, int clientAreaHeight);
					D3DContext(const D3DContext& other) = delete;
		D3DContext&	operator=(const D3DContext& rhs) = delete;
					D3DContext(D3DContext&& other) = default;
		D3DContext&	operator=(D3DContext&& rhs) = default;
					~D3DContext();
		/*Clears the RenderTargetView (default color is Cornflower Blue) and the DepthStencilView*/
		void		Clear(const DirectX::XMVECTORF32& color = { 0.392f, 0.584f, 0.929f, 1.0f });
		/*Presents the rendered image to the user*/
		void		Present();
		ID3D11Device&	GetDevice() const noexcept { return *m_device.Get(); }
		ID3D11DeviceContext& GetDeviceContext() const noexcept { return *m_deviceContext.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3D11Device>			m_device{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>		m_deviceContext{ nullptr};
		D3D_FEATURE_LEVEL								m_featureLevel{};
		UINT											m_multiSamplingCount{ 4 };
		UINT											m_multiSamplingQualityLevelCount{ 0 };
		bool											m_isMultiSamplingEnabled{ true };
		Microsoft::WRL::ComPtr<IDXGISwapChain1>			m_swapChain{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	m_renderTargetView{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11Texture2D>			m_depthStencilBuffer{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	m_depthStencilView{ nullptr };
		D3D11_VIEWPORT									m_viewPort{};

		void											InitializeDevice();
		void											CheckMultiSamplingQualityLevels();
		void											InitializeSwapChain(const HWND windowHandle, int clientAreaWidth, int clientAreaHeight);
		void											InitializeRenderTargetView();
		void											InitializeDepthStencilView(int clientAreaWidth, int clientAreaHeight);
		void											BindViewsToOutputMerger();
		void											SetupViewPort(int clientAreaWidth, int clientAreaHeight);
	};
}
}
}
