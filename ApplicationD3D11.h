#pragma once
#include "ApplicationBase.h"
#include "Timer.h"
#include <wrl/client.h>
#include "RenderD3D11.h"
#include "HexMap.h"
#include "Camera.h"

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class ApplicationD3D11 :
	public Application
{
public:
	ApplicationD3D11(const TCHAR* titleName = _T("D3D11App"), const TCHAR* className = _T("WndClass"));
	virtual ~ApplicationD3D11();

	bool InitApp(HINSTANCE hinstance) final;

	RenderD3D11* GetRender() { return m_render.get(); }

	static ApplicationD3D11* GetApp() { return static_cast<ApplicationD3D11*>(Application::GetApp()); }

	void OnMouseMove(WPARAM btnState, int x, int y);

protected:
	bool InitDirectX3D11();
	void OnResize();
	virtual LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override;

	virtual void OnFrame() override;

	void UpdateScene(float deltaTime);
	void RenderScene();

	void CalculateFrameStats();

private:
	
	bool m_AppPaused;
	bool m_Minimized;
	bool m_Maximized;
	bool m_Resizing;

	Timer* m_pTimer;

	std::unique_ptr<RenderD3D11> m_render;

	HexMap m_Hex;

	Camera m_mainCamera;
	
	POINT mLastMousePos;
};

