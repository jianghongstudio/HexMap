#include "stdafx.h"
#include "RenderToScreen.h"
#include "DirectXTex.h"


BlurDemo::BlurDemo(const TCHAR * titleName, const TCHAR * className):ApplicationD3D11(titleName,className)
{
}

BlurDemo::~BlurDemo()
{
}

void BlurDemo::BuildGeometryBuffer(ID3D11Device * device)
{
	m_Vertices = {
		{ { -1.f,1.f,0 },{ 0.f,0.f,-1.f },{ 0.f,0.f } },
		{ { 1.f,1.f,0 },{ 0.f,0.f,-1.f },{ 1.f,0.f } },
		{ { 1.f,-1.f,0 },{ 0.f,0.f,-1.f },{ 1.f,1.f } },
		{ { -1.f,-1.f,0 },{ 0.f,0.f,-1.f },{ 0.f,1.f } }
	};

	m_Indices = { 0,1,2,0,2,3 };

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexType) * m_Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = m_Vertices.data();
	HR(m_render->GetD3DDevice()->CreateBuffer(&vbd, &vinitData, &m_verticesBuffer));

	D3D11_BUFFER_DESC ibd;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.CPUAccessFlags = 0;
	ibd.ByteWidth = sizeof(UINT)*m_Indices.size();
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &m_Indices[0];
	HR(device->CreateBuffer(&ibd, &iinitData, m_indicesBuffer.ReleaseAndGetAddressOf()));
}

void BlurDemo::Draw(ID3D11DeviceContext * context, const Camera * camera)
{
	context->IASetInputLayout(m_renderFX->GetInputLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto renderFX = static_cast<FXBasicEffect*>(m_renderFX);

	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	auto VB = m_verticesBuffer.Get();


	//set constants
	XMMATRIX identity = XMMatrixIdentity();
	renderFX->SetEyePosW(camera->GetPosition());
	context->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
	context->IASetIndexBuffer(m_indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	renderFX->SetWorld(identity);
	renderFX->SetWorldInvTranspose(identity);
	renderFX->SetWorldViewProjMatrix(identity);
	renderFX->SetTexTransform(identity);
	renderFX->SetDiffuseMap(m_diffuseMap.Get());

	ID3DX11EffectTechnique* activeTech = renderFX->Light0Tex;

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		activeTech->GetPassByIndex(p)->Apply(0, context);
		context->DrawIndexed(m_Indices.size(), 0, 0);
	}
}

bool BlurDemo::InitApp(HINSTANCE hinstance)
{
	if (!ApplicationD3D11::InitApp(hinstance))
	{
		return false;
	}
	TexMetadata wall_texMeta;
	ScratchImage wall_diffuseImage;
	HR(LoadFromDDSFile(L"Textures/brick01.dds", 0, &wall_texMeta, wall_diffuseImage));
	HR(CreateShaderResourceView(m_render->GetD3DDevice(), wall_diffuseImage.GetImages(), wall_diffuseImage.GetImageCount(), wall_texMeta, &m_diffuseMap));
	this->Initialize(m_render->GetD3DDeviceContext());
	return true;
}

void BlurDemo::RenderScene()
{
	m_render->Clear();
	Draw(m_render->GetD3DDeviceContext(), &m_mainCamera);
	m_render->Present();
}

void BlurDemo::BindFX(ID3D11Device * device)
{
	m_renderFX = FXEffectsFactory::GetFXBasicEffect(device);
}
