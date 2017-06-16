#pragma once

#include <string>

namespace DarEngine {
	//TODO: refactor to use composition over inheritance, use std::function to implement callbacks/events
	class Win32Application
	{
	public:
		explicit						Win32Application(HINSTANCE instanceHandle, UINT clientAreaWidth, UINT clientAreaHeight, std::wstring applicationWindowTitle);
		virtual							~Win32Application() = default;
										Win32Application(const Win32Application& other) = delete;
										Win32Application(Win32Application&& other) noexcept = delete;
										Win32Application& operator=(const Win32Application& other) = delete;
										Win32Application& operator=(Win32Application&& other) noexcept = delete;

		int								Run();
		LRESULT							ProcessWindowMessage(HWND windowHandle, UINT uMessage, WPARAM wParam, LPARAM lParam);
	protected:
		//TODO: move to private and expose getters only, for changing these values introduce new methods like ChangeClientAreaWidth
		UINT							clientAreaWidth;
		UINT							clientAreaHeight;
		std::wstring					applicationWindowTitle;

		void							QuitApplication(int exitCode);
	private:
		DWORD							applicationWindowStyle{ WS_OVERLAPPEDWINDOW };
		HINSTANCE						applicationInstanceHandle{ nullptr };
		HWND							applicationWindowHandle{ nullptr };

		virtual void					OnApplicationInitialization() {};
		virtual void					OnApplicationQuit() {};
		virtual void					OnMessageLoopTick() {};
		virtual LRESULT					DoProcessWindowMessage(HWND windowHandle, UINT uMessage, WPARAM wParam, LPARAM lParam);
		bool							InitializeWindow();
		bool							InitializeWindowClass(WNDCLASSEX& windowClass) const;
		HWND							CreateApplicationWindow(WNDCLASSEX windowClass) const;
		void							ProcessApplicationMessages(MSG& message);
		bool							IsApplicationInitialized() const;
	};
}

