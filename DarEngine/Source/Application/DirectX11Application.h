#pragma once

#include "Win32Application.h"

namespace DarEngine
{
	const std::array<D3D_DRIVER_TYPE, 3> supportedDriverTypes{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	const std::array<D3D_FEATURE_LEVEL, 2> supportedFeatureLevels{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	/*
	 * Single window DirectX 11 application
	 * Derive from this class to implement your game
	 */
	class DirectX11Application : public Win32Application
	{
	public:
					DirectX11Application(HINSTANCE instanceHandle, UINT clientAreaWidth, UINT clientAreaHeight, const std::wstring& applicationWindowTitle);
		virtual		~DirectX11Application();
					DirectX11Application(const DirectX11Application& other) = delete;
					DirectX11Application(DirectX11Application&& other) noexcept = delete;
					DirectX11Application& operator=(const DirectX11Application& other) = delete;
					DirectX11Application& operator=(DirectX11Application&& other) noexcept = delete;

	private:
		Microsoft::WRL::ComPtr<ID3D11Device>				device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>			immediateDeviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain>				swapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		renderTargetView;
		D3D_DRIVER_TYPE										driverType{};
		D3D_FEATURE_LEVEL									featureLevel{};
		D3D11_VIEWPORT										viewport{};

		void				Update(float deltaTime);
		void				Render(float deltaTime);
		/*
		 * Callback method for updating game state
		 */
		virtual void		OnUpdate(float deltaTime) {};
		/*
		 * Callback method for rendering game state
		 */
		virtual void		OnRender(float deltaTime) {};
		void				OnApplicationInitialization() final override;
		void				OnMessageLoopTick() final override;
		bool				InitializeDirect3D();
		bool				InitializeDeviceAndSwapChain();
		bool				InitializeRenderTargetView();
		void				InitializeViewport();
		
	};
}