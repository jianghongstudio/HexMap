#include "stdafx.h"
#include "ApplicationD3D11.h"
#include "DirectXMath.h"
#include "DirectXColors.h"
#include "d3dcompiler.h"
using namespace DirectX;

#ifdef _WINDOWS
#include "TimerWin32.h"
#endif // _WINDOWS

ApplicationD3D11::ApplicationD3D11(const TCHAR* titleName, const TCHAR* className)
	: Application(titleName, className)
	, m_Enable4xMsaa(false)
	, m_4xMsaaQuality(0)
	, m_AppPaused(false)
	, m_Minimized(false)
	, m_Maximized(false)
	, m_Resizing(false)
	, m_pTimer(nullptr)
	, m_pFX(nullptr)
	, m_pTech(nullptr)
	, m_pfxWorldViewProj(nullptr)
	, m_pWireFrameRS(nullptr)
{
#ifdef _WINDOWS
	m_pTimer = new TimerWin32();
#endif // 
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_View, I);
	XMStoreFloat4x4(&m_World, I);
	XMStoreFloat4x4(&m_Proj, I);
}


ApplicationD3D11::~ApplicationD3D11()
{
	if (m_pTimer)
	{
		delete m_pTimer;
		m_pTimer = nullptr;
	}
	ReleaseCOM(m_pWireFrameRS);
	ReleaseCOM(m_pFX);
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

bool ApplicationD3D11::InitDirectX3D11()
{
	m_render = std::make_unique<RenderD3D11>();
	m_render->SetWindow(m_hWnd);
	m_render->CreateDeviceAndContext();
	
	OnResize();
	BuildGeometryBuffer();
	BuildFX();
	BuildVertexLayout();
	return true;
}

void ApplicationD3D11::OnResize()
{
	m_render->OnResize(m_ClientWidth, m_ClientHeight);
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*XM_PI, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);
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
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		return 0;
	case WM_MOUSEMOVE:
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
	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(0, 0, 30.f, 1.f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_View, V);
}

void ApplicationD3D11::RenderScene()
{
	m_render->Clear();
	auto context = m_render->GetD3DDeviceContext();
	context->IASetInputLayout(m_InputLayout.Get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	auto VB = m_BoxVB.Get();
	context->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
	context->IASetIndexBuffer(m_BoxIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	//set constants
	XMMATRIX world = XMLoadFloat4x4(&m_World);
	XMMATRIX view = XMLoadFloat4x4(&m_View);
	XMMATRIX proj = XMLoadFloat4x4(&m_Proj);
	XMMATRIX worldViewProj = world*view*proj;
	m_pfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pTech->GetPassByIndex(p)->Apply(0, context);

		context->RSSetState(m_pWireFrameRS);
		context->DrawIndexed(m_Indices.size(), 0, 0);
	}


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

void ApplicationD3D11::BuildFX()
{
	// Compile Effect in runtime
	/*UINT shaderFlag = 0;
#if defined(DEBUG)||defined(_DEBUG)
	shaderFlag |= D3D10_SHADER_DEBUG;
	shaderFlag |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
	ID3D10Blob* compileShader = nullptr;
	ID3D10Blob* compileMsg = nullptr;
	HRESULT hr = D3DX11CompileEffectFromFile(L"FX/color.fx", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, shaderFlag, 0, m_pDevice, &mfx, &compileMsg);
	if (FAILED(hr))
	{
		if (compileMsg != 0)
		{
			MessageBoxA(0, (char*)compileMsg->GetBufferPointer(), 0, 0);
		}
	}
	ReleaseCOM(compileMsg);*/

	// Compile Effect in advance
	std::ifstream fin("fx/color.fxo", std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size,
		0, m_render->GetD3DDevice(), &m_pFX));
	m_pTech = m_pFX->GetTechniqueByIndex(0);
	m_pfxWorldViewProj = m_pFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void ApplicationD3D11::BuildGeometryBuffer()
{
	m_Vertices.clear();
	m_Indices.clear();
	GeometryGenerator::MeshData box;
	m_GeometryGenerator.CreateBox(10, 10, 10,box);

	std::vector<Vertex> Vertices(box.Vertices.size());
	for (size_t i = 0;i<Vertices.size();i++)
	{
		Vertices[i].Pos = box.Vertices[i].Position;
		Vertices[i].Color = XMFLOAT4((const float*)&Colors::Red);
	}

	
	m_Vertices.insert(m_Vertices.end(), Vertices.begin(), Vertices.end());
	m_Indices.insert(m_Indices.end(), box.Indices.begin(), box.Indices.end());
	D3D11_BUFFER_DESC vbd;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.CPUAccessFlags = 0;
	vbd.ByteWidth = sizeof(Vertex)*m_Vertices.size();
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &m_Vertices[0];

	HR(m_render->GetD3DDevice()->CreateBuffer(&vbd, &vinitData, m_BoxVB.ReleaseAndGetAddressOf()));

	D3D11_BUFFER_DESC ibd;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.CPUAccessFlags = 0;
	ibd.ByteWidth = sizeof(UINT)*m_Indices.size();
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &m_Indices[0];
	HR(m_render->GetD3DDevice()->CreateBuffer(&ibd, &iinitData, m_BoxIB.ReleaseAndGetAddressOf()));
}

void ApplicationD3D11::BuildVertexLayout()
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	m_pTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(m_render->GetD3DDevice()->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, m_InputLayout.ReleaseAndGetAddressOf()));
}

float ApplicationD3D11::AspectRatio()const
{
	return static_cast<float>(m_ClientWidth) / m_ClientHeight;
}
