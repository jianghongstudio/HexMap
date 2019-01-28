#pragma once
#include <windows.h>
#include <tchar.h>
class Application
{
public:
	Application(const TCHAR* titleName = _T("Title"),const TCHAR* className = _T("WndClass"));
	virtual ~Application();

	// disable copy
	Application(const Application&) = delete;
	Application& operator = (const Application&) = delete;

	virtual bool InitApp(HINSTANCE hinstance);
	void Run();

	virtual LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	static Application* GetApp();
	static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	virtual bool CreateMainWindows(HINSTANCE hInstance);
	virtual void OnFrame() = 0;
protected:
	const TCHAR*	   m_szTitle;
	const TCHAR*	   m_WndClassName;
	HINSTANCE  m_hAppInstance;
	HWND	   m_hWnd;

	UINT m_ClientWidth;
	UINT m_ClientHeight;
};

