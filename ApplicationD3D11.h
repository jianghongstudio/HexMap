#pragma once
#include "ApplicationBase.h"
#include "Timer.h"
#include <wrl/client.h>
#ifdef USING_ORIGINAL_RENDER
#include "OriginalRenderD3D11.h"
#else
#include "RenderD3D11.h"
#endif // USING_ORIGINAL_RENDER
#include "HexMap.h"
#include "Camera.h"
#include "Crate.h"

class ApplicationD3D11 :
	public Application
{
public:
	ApplicationD3D11(_In_ const TCHAR* titleName = _T("D3D11App"), _In_ const TCHAR* className = _T("WndClass"));
	virtual ~ApplicationD3D11();

	bool InitApp(HINSTANCE hinstance) final;
#ifdef USING_ORIGINAL_RENDER
	OriginalRenderD3D11* GetRender() { return m_render.get(); }
#else
	RenderD3D11* GetRender() { return m_render.get(); }
#endif // USING_ORIGINAL_RENDER


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
#ifdef USING_ORIGINAL_RENDER
	std::unique_ptr<OriginalRenderD3D11> m_render;
#else
	std::unique_ptr<RenderD3D11> m_render;
#endif // USING_ORIGINAL_RENDER


	HexMap m_Hex;
	PrimitiveBox m_Box;
	PrimitiveRectangular m_floor;
	PrimitiveRectangular m_wall;
	Crate m_Crate;

	Camera m_mainCamera;
	
	POINT mLastMousePos;
};

