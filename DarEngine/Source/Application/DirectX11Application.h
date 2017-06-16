#pragma once

#include "Win32Application.h"

namespace DarEngine
{
	class DirectX11Application : public Win32Application
	{
	public:
						DirectX11Application(HINSTANCE instanceHandle, UINT clientAreaWidth, UINT clientAreaHeight, const std::wstring& applicationWindowTitle);
		virtual			~DirectX11Application();
						DirectX11Application(const DirectX11Application& other) = delete;
						DirectX11Application(DirectX11Application&& other) noexcept = delete;
						DirectX11Application& operator=(const DirectX11Application& other) = delete;
						DirectX11Application& operator=(DirectX11Application&& other) noexcept = delete;

	protected:

	private:
		void							OnMessageLoopTick() final override;
		virtual void					OnTick(float deltaTime) {};
	};
}