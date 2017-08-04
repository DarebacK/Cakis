#pragma once

#include "Clock.h"
#include "Win32Utilities/Window.h"

namespace DE
{

	class Game
	{
	public:
								Game(HINSTANCE instanceHandle, const std::wstring& windowTitle, int showCommand);
								Game(const Game& other) = delete;
								Game(Game&& other) = default;
								Game& operator=(const Game& rhs) = delete;
								Game& operator=(Game&& rhs) = default;
								~Game() = default;
		void					Run();
		void					Exit();

	private:
		HINSTANCE							m_instanceHandle{};
		Win32Utilities::Window				m_window;
		Clock								m_clock{};
		bool								m_isRunning{ false };
		// D3D attributes TODO: extract them to one class
		Microsoft::WRL::ComPtr<ID3D11Device>		m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>	m_d3dDeviceContext;
		D3D_FEATURE_LEVEL							m_d3dFeatureLevel{};
		UINT										m_d3dMultiSamplingCount{ 4 };
		UINT										m_d3dMultiSamplingQualityLevelCount{};
		bool										m_d3disMultiSamplingEnabled{ true };
		Microsoft::WRL::ComPtr<IDXGISwapChain1>		m_dxgiSwapChain;

		void								Initialize();
		void								InitializeD3D();
		void								RunGameLoop();
		void								Shutdown();
	};


}
