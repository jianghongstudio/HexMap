#include "stdafx.h"
#include "ApplicationD3D11.h"
#include "DirectXMath.h"
#include "DirectXColors.h"
#include "d3dcompiler.h"
#include <WindowsX.h>
using namespace DirectX;

#ifdef _WINDOWS
#include "TimerWin32.h"
#endif // _WINDOWS

ApplicationD3D11::ApplicationD3D11(const TCHAR* titleName, const TCHAR* className)
	: Application(titleName, className)
	, m_AppPaused(false)
	, m_Minimized(false)
	, m_Maximized(false)
	, m_Resizing(false)
	, m_pTimer(nullptr)
{
#ifdef _WINDOWS
	m_pTimer = new TimerWin32();
#endif // 
	m_mainCamera.SetPosition(0, 10,-30);
	m_mainCamera.LookAt(m_mainCamera.GetPosition(), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 1.f, 0.f));
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;
}


ApplicationD3D11::~ApplicationD3D11()
{
	if (m_pTimer)
	{
		delete m_pTimer;
		m_pTimer = nullptr;
	}
}

bool ApplicationD3D11::InitApp(HINSTANCE hinstance)
{
	if (!CreateMainWindows(hinstance))
	{
		return false;
	}
	if (!InitDirectX3D11())
	{
		return false;
	}

	return true;
}

void ApplicationD3D11::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(
			0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(
			0.25f*static_cast<float>(y - mLastMousePos.y));
		m_mainCamera.Pitch(dy);
		m_mainCamera.RotateY(dx);
	}
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void ApplicationD3D11::OnMouseButtonDown(WPARAM btnState, int x, int y)
{
}

bool ApplicationD3D11::InitDirectX3D11()
{
#ifdef USING_ORIGINAL_RENDER
	m_render = std::make_unique<OriginalRenderD3D11>();
	m_render->SetWindow(m_hWnd, m_ClientWidth,m_ClientHeight);
	m_render->CreateDeviceAndContext();

	OnResize();
#else
	m_render = std::make_unique<RenderD3D11>();
	m_render->SetWindow(m_hWnd);
	m_render->CreateDeviceAndContext();

	OnResize();
#endif // USING_ORIGINAL_RENDER

	return true;
}

void ApplicationD3D11::OnResize()
{
	m_render->OnResize(m_ClientWidth, m_ClientHeight);

	m_mainCamera.SetLens(XM_PIDIV4, m_render->AspectRatio(), 1.f, 1000.f);
}

LRESULT ApplicationD3D11::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_AppPaused = true;
			m_pTimer->Stop();
		}
		else
		{
			m_AppPaused = false;
			m_pTimer->Start();
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		m_ClientWidth = LOWORD(lParam);
		m_ClientHeight = HIWORD(lParam);
		if (m_render.get())
		{
			if (wParam == SIZE_MINIMIZED)
			{
				m_AppPaused = true;
				m_Minimized = true;
				m_Maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_AppPaused = false;
				m_Minimized = false;
				m_Maximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (m_Minimized)
				{
					m_AppPaused = false;
					m_Minimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (m_Maximized)
				{
					m_AppPaused = false;
					m_Maximized = false;
					OnResize();
				}
				else if (m_Resizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		m_AppPaused = true;
		m_Resizing = true;
		m_pTimer->Stop();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		m_AppPaused = false;
		m_Resizing = false;
		m_pTimer->Start();
		OnResize();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseButtonDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_KEYDOWN:
	{
		int vk = static_cast<int>(wParam);
		switch (vk)
		{
		case VK_UP:
			m_mainCamera.Walk(0.1);
			break;
		case VK_DOWN:
			m_mainCamera.Walk(-0.1);
			break;
		default:
			break;
		}
	}
		return 0;
	case WM_KEYUP:
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

void ApplicationD3D11::OnFrame()
{
	m_pTimer->Tick();
	if (!m_AppPaused)
	{
		CalculateFrameStats();
		UpdateScene(m_pTimer->DeltaTime());
		RenderScene();
	}
	else
	{
		Sleep(100);
	}
}

void ApplicationD3D11::UpdateScene(float deltaTime)
{
	m_mainCamera.UpdateViewMatrix();
}

void ApplicationD3D11::RenderScene()
{
	m_render->Clear();
	
	m_render->Present();
}

void ApplicationD3D11::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((m_pTimer->TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << m_szTitle << L"    "
			<< L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(m_hWnd, outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}
