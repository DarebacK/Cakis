#include "stdafx.h"
#include "Game.h"
#include "Exception.h"

void DE::Game::Run()
{
	try
	{
		Initialize();
		RunGameLoop();
	}
	catch (Exception ex)
	{
		MessageBox(m_window.GetHandle(), ex.message.c_str(), L"Fatal error", MB_ABORTRETRYIGNORE);
		throw;	
	}

	Shutdown(); //TODO: call Shutdown before throw or not?
}

void DE::Game::Exit()
{
	m_isRunning = false;
}

DE::Game::Game(HINSTANCE instanceHandle, const std::wstring& windowTitle, int showCommand)
	:m_instanceHandle {instanceHandle}, m_window{instanceHandle, windowTitle, showCommand}
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
}

void DE::Game::Initialize()
{
	m_window.Show();
	m_clock.Reset();
	InitializeD3D();
}

void DE::Game::InitializeD3D()
{
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined( _DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	std::array<D3D_FEATURE_LEVEL, 1> featureLevels = {
		D3D_FEATURE_LEVEL_11_1 ,
	};
	HRESULT hr;
	if (FAILED(hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE,
		NULL, createDeviceFlags, featureLevels.data(), featureLevels.size(),
		D3D11_SDK_VERSION, &m_d3dDevice, &m_d3dFeatureLevel,
		&m_d3dDeviceContext)))
	{
		if (FAILED(hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE,
			NULL, createDeviceFlags, NULL, NULL,
			D3D11_SDK_VERSION, &m_d3dDevice, &m_d3dFeatureLevel,
			&m_d3dDeviceContext)))
		{
			throw Exception{ L"D3D11CreateDevice() failed" };
		}	
	}
}

void DE::Game::RunGameLoop()
{
	m_isRunning = true;

	MSG message;
	ZeroMemory(&message, sizeof(message));

	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			m_clock.Update();
			auto deltaTime = ComputeDeltaTime(m_clock);
			//TODO: update and draw
		}
	}
}

void DE::Game::Shutdown()
{
	//TODO: implement
}
