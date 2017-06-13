#pragma once

#include <string>

//TODO: separate Win32 app and DirectX app
class ADirectXApplication
{
public:
	explicit						ADirectXApplication(HINSTANCE instanceHandle, UINT clientAreaWidth, UINT clientAreaHeight, std::wstring applicationWindowTitle);
	virtual							~ADirectXApplication();

	int								Run();
	virtual LRESULT					ProcessWindowMessage(HWND windowHandle, UINT uMessage, WPARAM wParam, LPARAM lParam);
protected:
	// WIN32 ATTRIBUTES
	//TODO: move to private and expose getters only, for changing these values introduce new methods like ChangeClientAreaWidth
	UINT							clientAreaWidth;
	UINT							clientAreaHeight;
	std::wstring					applicationWindowTitle;

	virtual void					OnUpdate(float deltaTime) = 0;
	virtual void					OnRender(float deltaTime) = 0;
	virtual void					OnApplicationInitialization() = 0;
	virtual void					OnApplicationQuit() = 0;
	void							QuitApplication(int exitCode);
private:
	DWORD							applicationWindowStyle{ WS_OVERLAPPEDWINDOW };
	HINSTANCE						applicationInstanceHandle{ nullptr };
	HWND							applicationWindowHandle{ nullptr };

	bool							InitializeWindow();
	void							ProcessApplicationMessages(MSG& message);
	bool							IsApplicationInitialized() const;
};

