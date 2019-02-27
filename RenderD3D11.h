#pragma once
#include <wrl\client.h>
#include "d3d11.h"
#include "d3d11_1.h"
class RenderD3D11
{
public:
	RenderD3D11(DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT,
		UINT backBufferCount = 1,
		D3D_FEATURE_LEVEL minFeatureLevel = D3D_FEATURE_LEVEL_11_0);
	~RenderD3D11();

	bool CreateDeviceAndContext();
	void SetWindow(HWND window, UINT width = 0, UINT height = 0);

	void OnResize(UINT width, UINT height);

	float AspectRatio()const
	{
		return static_cast<float>(m_outputSize.right - m_outputSize.left) / (m_outputSize.bottom - m_outputSize.top);
	}

	inline ID3D11DeviceContext* GetD3DDeviceContext() const { return m_d3dImmediateContext.Get(); }

	void Clear();

	void Present();
protected:
	void GraphicResize();
private:

	Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dImmediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_d3dSwapChain;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> m_d3dSwapChain1;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_d3dDepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_d3dRenderTarget;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_d3dRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_d3dDepthStencilView;
	D3D11_VIEWPORT m_screenViewport;

	// Direct3D properties.
	DXGI_FORMAT                                     m_backBufferFormat;
	DXGI_FORMAT                                     m_depthBufferFormat;
	UINT                                            m_backBufferCount;
	D3D_FEATURE_LEVEL                               m_d3dMinFeatureLevel;
	D3D_FEATURE_LEVEL                               m_d3dFeatureLevel;

	HWND											m_window;
	RECT                                            m_outputSize;
	bool											m_enable4xMsaa;
	UINT											m_4xMsaaQuality;
};

