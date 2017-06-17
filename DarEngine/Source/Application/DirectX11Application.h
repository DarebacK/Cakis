#pragma once

#include "Win32Application.h"

namespace DarEngine
{
	class DirectX11Application : public Win32Application
	{
	public:
									DirectX11Application(HINSTANCE instanceHandle, UINT clientAreaWidth, UINT clientAreaHeight, const std::wstring& applicationWindowTitle);
		virtual						~DirectX11Application();
									DirectX11Application(const DirectX11Application& other) = delete;
									DirectX11Application(DirectX11Application&& other) noexcept = delete;
									DirectX11Application& operator=(const DirectX11Application& other) = delete;
									DirectX11Application& operator=(DirectX11Application&& other) noexcept = delete;
	private:
		ID3D11Device*				device{ nullptr };
		ID3D11DeviceContext*		immediateDeviceContext{ nullptr };
		IDXGISwapChain*				swapChain{ nullptr };
		ID3D11RenderTargetView*		renderTargetView{ nullptr };
		D3D_DRIVER_TYPE				driverType{};
		D3D_FEATURE_LEVEL			featureLevel{};
		D3D11_VIEWPORT				viewport{};

		virtual void				OnTick(float deltaTime) {};
		void						OnApplicationInitialization() final override;
		void						OnMessageLoopTick() final override;
		bool						InitializeDirect3D();
		
	};
}