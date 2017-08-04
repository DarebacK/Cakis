#pragma once

namespace DE
{
	namespace WinUtilities
	{
		class Window
		{
		public:
					Window(HINSTANCE instanceHandle, const std::wstring& title, int showCommand);
					Window(const Window& other) = delete;
					Window(Window&& other) noexcept = default;
					Window& operator=(const Window& other) = delete;
					Window& operator=(Window&& other) noexcept = default;
					~Window() = default;
			HWND	GetHandle() const { return m_handle; }
			int		GetClientAreaWidth() const { return m_clientAreaWidth; }
			int		GetClientAreaHeight() const { return m_clientAreaHeight; }
			void	Show() const;

		private:
			static const int		clientAreaDefaultWidth{ 1024 };
			static const int		clientAreaDefaultHeight{ 768 };

			HWND					m_handle{};
			WNDCLASSEX				m_class{};
			std::wstring			m_className{};
			std::wstring			m_title{};
			int						m_showCommand{};
			int						m_clientAreaWidth{ clientAreaDefaultWidth };
			int						m_clientAreaHeight{ clientAreaDefaultHeight };

			static	LRESULT WINAPI	WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
			void					InitializeClass(HINSTANCE instanceHandle);
			HWND					InitializeWindow(HINSTANCE instanceHandle);
		};
	}
}