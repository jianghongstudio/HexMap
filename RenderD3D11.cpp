#include "stdafx.h"
#include "RenderD3D11.h"
#include <assert.h>
#include <algorithm>
#include <d3d11_1.h>

using Microsoft::WRL::ComPtr;

RenderD3D11::RenderD3D11(DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthBufferFormat, UINT backBufferCount, D3D_FEATURE_LEVEL minFeatureLevel)
	:m_backBufferFormat(backBufferFormat)
	,m_depthBufferFormat(depthBufferFormat)
	,m_backBufferCount(backBufferCount)
	,m_d3dMinFeatureLevel(minFeatureLevel)
	,m_window(nullptr)
	,m_d3dFeatureLevel(D3D_FEATURE_LEVEL_11_0)
	,m_outputSize{ 0, 0, 1, 1 }
	,m_screenViewport{}
	,m_enable4xMsaa(false)
{
}

RenderD3D11::~RenderD3D11()
{
}

bool RenderD3D11::CreateDeviceAndContext()
{
	UINT createDeviceFlag = 0;
#if defined(DEBUG)||defined(_DEBUG)
	createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlag, nullptr, 0, D3D11_SDK_VERSION, m_d3dDevice.ReleaseAndGetAddressOf(), &featureLevel, m_d3dContext.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(nullptr, _T("D3D11CreateDevice Failed!"), _T("error"), MB_OK);
		return false;
	}
	assert(featureLevel >= m_d3dMinFeatureLevel);

	HR(m_d3dDevice->CheckMultisampleQualityLevels(m_backBufferFormat, 4, &m_4xMsaaQuality));
	assert(m_4xMsaaQuality > 0);
	return true;
}

void RenderD3D11::SetWindow(HWND window, UINT width, UINT height)
{
	m_window = window;

	m_outputSize.left = m_outputSize.top = 0;
	m_outputSize.right = width;
	m_outputSize.bottom = height;
}

void RenderD3D11::Clear(const float* clearColor)
{
	
	auto context = this->GetD3DDeviceContext();
	auto renderTarget = this->GetRenderTargetView();
	auto depthStencil = this->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, clearColor);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	m_d3dContext->RSSetViewports(1, &m_screenViewport);
}


void RenderD3D11::OnResize(UINT width, UINT height)
{
	RECT newRc;
	newRc.left = newRc.top = 0;
	newRc.right = width;
	newRc.bottom = height;
	if (newRc == m_outputSize)
	{
		return;
	}

	m_outputSize = newRc;

	GraphicResize();
}

void RenderD3D11::Present()
{
	m_swapChain->Present(0, 0);
}

void RenderD3D11::GraphicResize()
{
	if (!m_window) return;
	// Clear the previous window size specific context.
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
	m_d3dRenderTargetView.Reset();
	m_d3dDepthStencilView.Reset();
	m_renderTarget.Reset();
	m_depthStencil.Reset();
	m_d3dContext->Flush();

	// Determine the render target size in pixels.
	UINT backBufferWidth = std::max<UINT>(m_outputSize.right - m_outputSize.left, 1);
	UINT backBufferHeight = std::max<UINT>(m_outputSize.bottom - m_outputSize.top, 1);
	if (!m_swapChain)
	{
		// Otherwise, create a new one using the same adapter as the existing Direct3D device.

		// This sequence obtains the DXGI factory that was used to create the Direct3D device above.
		ComPtr<IDXGIDevice1> dxgiDevice;
		HR(m_d3dDevice.As(&dxgiDevice));

		ComPtr<IDXGIAdapter> dxgiAdapter;
		HR(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

		ComPtr<IDXGIFactory1> dxgiFactory;
		HR(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

		ComPtr<IDXGIFactory2> dxgiFactory2;
		if (SUCCEEDED(dxgiFactory.As(&dxgiFactory2)))
		{
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
			swapChainDesc.Width = backBufferWidth;
			swapChainDesc.Height = backBufferHeight;
			swapChainDesc.Format = m_backBufferFormat;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = m_backBufferCount;
			if (m_enable4xMsaa)
			{
				swapChainDesc.SampleDesc.Count = 4;
				swapChainDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
			}
			else
			{
				swapChainDesc.SampleDesc.Count = 1;
				swapChainDesc.SampleDesc.Quality = 0;
			}
			swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
			fsSwapChainDesc.Windowed = TRUE;

			// Create a SwapChain from a Win32 window.
			HR(dxgiFactory2->CreateSwapChainForHwnd(
				m_d3dDevice.Get(),
				m_window,
				&swapChainDesc,
				&fsSwapChainDesc,
				nullptr, m_swapChain1.ReleaseAndGetAddressOf()
			));

			HR(m_swapChain1.As(&m_swapChain));
		}
		else
		{
			DXGI_SWAP_CHAIN_DESC sd;
			sd.BufferCount = 1;
			sd.BufferDesc.Format = m_backBufferFormat;
			sd.BufferDesc.RefreshRate.Denominator = 1;
			sd.BufferDesc.RefreshRate.Numerator = 60;
			sd.BufferDesc.Width = backBufferWidth;
			sd.BufferDesc.Height = backBufferHeight;
			sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.Flags = 0;
			sd.OutputWindow = m_window;
			sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			sd.Windowed = TRUE;
			if (m_enable4xMsaa)
			{
				sd.SampleDesc.Count = 4;
				sd.SampleDesc.Quality = m_4xMsaaQuality - 1;
			}
			else
			{
				sd.SampleDesc.Count = 1;
				sd.SampleDesc.Quality = 0;
			}
			HR(dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &sd, m_swapChain.ReleaseAndGetAddressOf()));

			dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_WINDOW_CHANGES);
		}
	}
	else
	{
		HR(m_swapChain->ResizeBuffers(m_backBufferCount, backBufferWidth, backBufferHeight,m_backBufferFormat,0));
	}

	HR(m_swapChain->GetBuffer(0, IID_PPV_ARGS(m_renderTarget.ReleaseAndGetAddressOf())));
	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, m_backBufferFormat);
	HR(m_d3dDevice->CreateRenderTargetView(m_renderTarget.Get(), &renderTargetViewDesc, m_d3dRenderTargetView.ReleaseAndGetAddressOf()));

	if (m_depthBufferFormat!=DXGI_FORMAT_UNKNOWN)
	{
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = backBufferWidth;
		depthStencilDesc.Height = backBufferHeight;
		depthStencilDesc.Format = m_depthBufferFormat;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;

		if (m_enable4xMsaa)
		{
			depthStencilDesc.SampleDesc.Count = 4;
			depthStencilDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
		}
		// No MSAA
		else
		{
			depthStencilDesc.SampleDesc.Count = 1;
			depthStencilDesc.SampleDesc.Quality = 0;
		}
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		HR(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, m_depthStencil.ReleaseAndGetAddressOf()));
		HR(m_d3dDevice->CreateDepthStencilView(m_depthStencil.Get(), nullptr, m_d3dDepthStencilView.ReleaseAndGetAddressOf()));
	}
	m_screenViewport = {
		0.0f,
		0.0f,
		static_cast<float>(backBufferWidth),
		static_cast<float>(backBufferHeight)
	};
}
