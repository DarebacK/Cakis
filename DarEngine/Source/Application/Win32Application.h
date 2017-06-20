#pragma once

namespace DarEngine {
	class Win32Application
	{
	public:
		explicit						Win32Application(HINSTANCE instanceHandle, UINT clientAreaWidth, UINT clientAreaHeight, std::wstring applicationWindowTitle);
		virtual							~Win32Application() = default;
										Win32Application(const Win32Application& other) = delete;
										Win32Application(Win32Application&& other) noexcept = delete;
										Win32Application& operator=(const Win32Application& other) = delete;
										Win32Application& operator=(Win32Application&& other) noexcept = delete;

		/*
		 * Performs application initialization and runs the message loop
		 * If the application is already running, a warning message is outputted
		 */
		int								Run();
		/*
		 * Posts quit message to the operating system
		 * OnApplicationQuit is called before the message is sent
		 */
		void							QuitApplication(int exitCode);
		/*
		 * Callback method for Win32 api messaging system
		 * Should NOT be called by the user
		 */
		LRESULT							ProcessWindowMessage(HWND windowHandle, UINT uMessage, WPARAM wParam, LPARAM lParam);
		UINT							GetClientAreaWidth() const;
		UINT							GetClientAreaHeight() const;
		const std::wstring&				GetAppplicationWindowTitle() const;
		HWND							GetApplicationWindowHandle() const;
	private:
		UINT							clientAreaWidth;
		UINT							clientAreaHeight;
		std::wstring					applicationWindowTitle;
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

