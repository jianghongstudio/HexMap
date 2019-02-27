#include "stdafx.h"
#include "RenderD3D11.h"
#include <algorithm>
#include "DirectXColors.h"
using namespace DirectX;
using namespace Microsoft::WRL;

RenderD3D11::RenderD3D11(DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthBufferFormat, UINT backBufferCount, D3D_FEATURE_LEVEL minFeatureLevel)
	:m_backBufferFormat(backBufferFormat)
	, m_depthBufferFormat(depthBufferFormat)
	, m_backBufferCount(backBufferCount)
	, m_d3dMinFeatureLevel(minFeatureLevel)
	, m_window(nullptr)
	, m_d3dFeatureLevel(D3D_FEATURE_LEVEL_11_0)
	, m_outputSize{ 0, 0, 1, 1 }
	, m_screenViewport{}
	, m_enable4xMsaa(false)
{
}

RenderD3D11::~RenderD3D11()
{
}

bool RenderD3D11::CreateDeviceAndContext()
{
	// Create the device and device context.

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,                 // default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		0,                 // no software device
		createDeviceFlags,
		0, 0,              // default feature level array
		D3D11_SDK_VERSION,
		&m_d3dDevice,
		&featureLevel,
		&m_d3dImmediateContext);

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	HR(m_d3dDevice->CheckMultisampleQualityLevels(
		m_backBufferFormat, 4, &m_4xMsaaQuality));
	assert(m_4xMsaaQuality > 0);

	// The remaining steps that need to be carried out for d3d creation
	// also need to be executed every time the window is resized.  So
	// just call the OnResize method here to avoid code duplication.

	GraphicResize();

	return true;
}

void RenderD3D11::SetWindow(HWND window, UINT width, UINT height)
{
	m_window = window;
	m_outputSize.left = m_outputSize.top = 0;
	m_outputSize.right = width;
	m_outputSize.bottom = height;
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

void RenderD3D11::Clear()
{
	m_d3dImmediateContext->ClearRenderTargetView(m_d3dRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Silver));
	m_d3dImmediateContext->ClearDepthStencilView(m_d3dDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	// Bind the render target view and depth/stencil view to the pipeline.

	auto renderTargetView = m_d3dRenderTargetView.Get();
	m_d3dImmediateContext->OMSetRenderTargets(1, &renderTargetView, m_d3dDepthStencilView.Get());
}

void RenderD3D11::Present()
{
	HR(m_d3dSwapChain->Present(0, 0));
}

void RenderD3D11::GraphicResize()
{
	if (!m_window) return;
	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_d3dImmediateContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
	m_d3dDepthStencilBuffer.Reset();
	m_d3dRenderTarget.Reset();
	m_d3dRenderTargetView.Reset();
	m_d3dDepthStencilView.Reset();
	m_d3dImmediateContext->Flush();

	UINT backBufferWidth = std::max<UINT>(m_outputSize.right - m_outputSize.left, 1);
	UINT backBufferHeight = std::max<UINT>(m_outputSize.bottom - m_outputSize.top, 1);

	if (!m_d3dSwapChain.Get())
	{
		// Otherwise, create a new one using the same adapter as the existing Direct3D device.

		// This sequence obtains the DXGI factory that was used to create the Direct3D device above.
		ComPtr<IDXGIDevice1> dxgiDevice;
		HR(m_d3dDevice.As(&dxgiDevice));

		ComPtr<IDXGIAdapter> dxgiAdapter;
		HR(dxgiDevice->GetAdapter(&dxgiAdapter));

		ComPtr<IDXGIFactory1> dxgiFactory;
		HR(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));

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
				nullptr, &m_d3dSwapChain1
			));

			HR(m_d3dSwapChain1.As(&m_d3dSwapChain));
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
			HR(dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &sd,&m_d3dSwapChain));

			dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_WINDOW_CHANGES);
		}
	}
	else
	{
		HR(m_d3dSwapChain->ResizeBuffers(m_backBufferCount, backBufferWidth, backBufferHeight, m_backBufferFormat, 0));
	}

	// Resize the swap chain and recreate the render target view.
	HR(m_d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &m_d3dRenderTarget));
	HR(m_d3dDevice->CreateRenderTargetView(m_d3dRenderTarget.Get(), 0, &m_d3dRenderTargetView));

	// Create the depth/stencil buffer and view.

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = backBufferWidth;
	depthStencilDesc.Height = backBufferHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = m_depthBufferFormat;

	// Use 4X MSAA? --must match swap chain MSAA values.
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

	HR(m_d3dDevice->CreateTexture2D(&depthStencilDesc, 0, &m_d3dDepthStencilBuffer));
	HR(m_d3dDevice->CreateDepthStencilView(m_d3dDepthStencilBuffer.Get(), 0, &m_d3dDepthStencilView));

	// Set the viewport transform.

	m_screenViewport.TopLeftX = 0;
	m_screenViewport.TopLeftY = 0;
	m_screenViewport.Width = static_cast<float>(backBufferWidth);
	m_screenViewport.Height = static_cast<float>(backBufferHeight);
	m_screenViewport.MinDepth = 0.0f;
	m_screenViewport.MaxDepth = 1.0f;

	m_d3dImmediateContext->RSSetViewports(1, &m_screenViewport);
}
