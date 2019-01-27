#include "stdafx.h"
#include "ApplicationBase.h"
using namespace std;

extern unique_ptr<ApplicationBase> s_pAppInstance;

ApplicationBase::ApplicationBase(const TCHAR* titleName, const TCHAR* className)
	: m_szTitle(titleName)
	, m_WndClassName(className)
	, m_ClientWidth(800)
	, m_ClientHeight(600)
{
	
}


ApplicationBase::~ApplicationBase()
{
}

bool ApplicationBase::InitApp(HINSTANCE hinstance)
{
	if (!CreateMainWindows(hinstance))
	{
		return false;
	}
	return true;
}

void ApplicationBase::Run()
{

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			OnFrame();
		}
	}
}

ApplicationBase* ApplicationBase::GetApp()
{
	return s_pAppInstance.get();
}

LRESULT ApplicationBase::StaticWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto pApplication = GetApp();
	if (pApplication)
	{
		return pApplication->WndProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

bool ApplicationBase::CreateMainWindows(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = StaticWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = m_WndClassName;
	wcex.hIconSm = wcex.hIcon;

	if (!RegisterClassEx(&wcex))
	{
		return false;
	}

	RECT rc = { 0, 0, m_ClientWidth, m_ClientHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPED| WS_CAPTION| WS_SYSMENU| WS_MINIMIZEBOX| WS_MAXIMIZEBOX, false);
	HWND hWnd = CreateWindow(m_WndClassName, m_szTitle, WS_OVERLAPPED| WS_CAPTION| WS_SYSMENU| WS_MINIMIZEBOX| WS_MAXIMIZEBOX,
		0, 0, rc.right-rc.left, rc.bottom-rc.top, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return false;
	}

	m_hAppInstance = hInstance;
	m_hWnd = hWnd;

	return true;
}


LRESULT ApplicationBase::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	auto str = _T("HelloWorld");
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		TextOut(hdc, 0, 0, str, _tcslen(str));
		EndPaint(hwnd, &ps);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}