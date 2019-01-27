#pragma once
#include "ApplicationBase.h"
#include "d3d11.h"
#include "Timer.h"
#include "d3dx11effect.h"
#include "GeometryGenerator.h"
#include "RenderD3D11.h"
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class ApplicationD3D11 :
	public ApplicationBase
{
public:
	ApplicationD3D11(const TCHAR* titleName = _T("D3D11App"), const TCHAR* className = _T("WndClass"));
	virtual ~ApplicationD3D11();

	bool InitApp(HINSTANCE hinstance) final;

protected:
	//bool InitDirectX3D11();
	void OnResize();
	virtual LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override;

	virtual void OnFrame() override;

	void UpdateScene(float deltaTime);
	void RenderScene();

	void CalculateFrameStats();

	void BuildFX();
	void BuildGeometryBuffer();
	void BuildVertexLayout();

	float AspectRatio()const;
private:
	
	//bool m_Enable4xMsaa;
	bool m_AppPaused;
	bool m_Minimized;
	bool m_Maximized;
	bool m_Resizing;

	Timer* m_pTimer;

	//UINT m_4xMsaaQuality;

	/*ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pImmdiateContext;
	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11Texture2D* m_pDepthStencilBuffer;
	ID3D11DepthStencilView* m_pDepthStencilView;
	D3D11_VIEWPORT m_ScreenViewport;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pBoxVB;
	ID3D11Buffer* m_pBoxIB;
	ID3D11RasterizerState* m_pWireFrameRS;*/

	std::unique_ptr<RenderD3D11> m_d3dRender;

	//Effect
	ID3DX11Effect* m_pFX;
	ID3DX11EffectTechnique* m_pTech;
	ID3DX11EffectMatrixVariable*  m_pfxWorldViewProj;

	//GeometryGenerator
	GeometryGenerator m_GeometryGenerator;

	XMFLOAT4X4 m_World;
	XMFLOAT4X4 m_View;
	XMFLOAT4X4 m_Proj;

	std::vector<UINT> m_Indices;
	std::vector<Vertex> m_Vertices;
};

