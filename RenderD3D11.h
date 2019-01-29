#pragma once
#include <wrl\client.h>
#include "d3d11.h"
#include "d3d11_1.h"
#include "DirectXMath.h"
#include "DirectXColors.h"
class RenderD3D11
{
public:
	RenderD3D11(DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT,
		UINT backBufferCount = 1,
		D3D_FEATURE_LEVEL minFeatureLevel = D3D_FEATURE_LEVEL_11_0);
	virtual ~RenderD3D11();

	bool CreateDeviceAndContext();
	void SetWindow(HWND window, UINT width = 0, UINT height = 0);
	void Clear(const float* clearColor = DirectX::Colors::CornflowerBlue);

	void OnResize(UINT width, UINT height);

	void Present();

	ID3D11Device*           GetD3DDevice() const { return m_d3dDevice.Get(); }
	//ID3D11Device1*          GetD3DDevice1() const { return m_d3dDevice1.Get(); }
	ID3D11DeviceContext*    GetD3DDeviceContext() const { return m_d3dContext.Get(); }
	//ID3D11DeviceContext1*   GetD3DDeviceContext1() const { return m_d3dContext1.Get(); }
	IDXGISwapChain*         GetSwapChain() const { return m_swapChain.Get(); }
	IDXGISwapChain1*        GetSwapChain1() const { return m_swapChain1.Get(); }
	D3D_FEATURE_LEVEL       GetDeviceFeatureLevel() const { return m_d3dFeatureLevel; }
	ID3D11Texture2D*        GetRenderTarget() const { return m_renderTarget.Get(); }
	ID3D11Texture2D*        GetDepthStencil() const { return m_depthStencil.Get(); }
	ID3D11RenderTargetView*	GetRenderTargetView() const { return m_d3dRenderTargetView.Get(); }
	ID3D11DepthStencilView* GetDepthStencilView() const { return m_d3dDepthStencilView.Get(); }
	DXGI_FORMAT             GetBackBufferFormat() const { return m_backBufferFormat; }
	DXGI_FORMAT             GetDepthBufferFormat() const { return m_depthBufferFormat; }
	D3D11_VIEWPORT          GetScreenViewport() const { return m_screenViewport; }
	UINT                    GetBackBufferCount() const { return m_backBufferCount; }

	float AspectRatio()const
	{
		return static_cast<float>(m_outputSize.right - m_outputSize.left) / (m_outputSize.bottom - m_outputSize.top);
	}

protected:
	void GraphicResize();
private:
	// Direct3D objects.
	Microsoft::WRL::ComPtr<ID3D11Device>            m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>     m_d3dContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>          m_swapChain;
	Microsoft::WRL::ComPtr<IDXGISwapChain1>          m_swapChain1;

	// Direct3D rendering objects. Required for 3D.
	Microsoft::WRL::ComPtr<ID3D11Texture2D>         m_renderTarget;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>         m_depthStencil;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_d3dRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_d3dDepthStencilView;
	D3D11_VIEWPORT                                  m_screenViewport;

	// Direct3D properties.
	DXGI_FORMAT                                     m_backBufferFormat;
	DXGI_FORMAT                                     m_depthBufferFormat;
	UINT                                            m_backBufferCount;
	D3D_FEATURE_LEVEL                               m_d3dMinFeatureLevel;

	// Cached device properties.
	HWND                                            m_window;
	D3D_FEATURE_LEVEL                               m_d3dFeatureLevel;
	RECT                                            m_outputSize;

	UINT											m_4xMsaaQuality;

	bool											m_enable4xMsaa;
};

