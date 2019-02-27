#include "stdafx.h"
#include "Crate.h"
using namespace DirectX;

Crate::Crate(UINT width, UINT height, UINT depth)
	:GeometricPrimitive()
	,m_width(width)
	,m_height(height)
	,m_depth(depth)
{
	mDirLights[0].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
	mDirLights[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	mDirLights[1].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Diffuse = XMFLOAT4(1.4f, 1.4f, 1.4f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);
	mDirLights[1].Direction = XMFLOAT3(-0.707f, 0.0f, 0.707f);

	mBoxMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mBoxMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mBoxMat.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
}

Crate::~Crate()
{
}

void Crate::BuildGeometryBuffer(_In_ ID3D11Device* device)
{
	m_Vertices.clear();
	m_Indices.clear();
	GeometryGenerator::MeshData box;
	GeometryGenerator::CreateBox(m_width, m_height, m_depth, box);

	m_Vertices.resize(box.Vertices.size());
	for (size_t i = 0; i < m_Vertices.size(); i++)
	{
		VertexTypes::ConvertFromGemometryVertex(m_Vertices[i], box.Vertices[i]);
	}

	m_Indices.insert(m_Indices.end(), box.Indices.begin(), box.Indices.end());
	D3D11_BUFFER_DESC vbd;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.CPUAccessFlags = 0;
	vbd.ByteWidth = sizeof(VertexType)*m_Vertices.size();
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &m_Vertices[0];

	HR(device->CreateBuffer(&vbd, &vinitData, m_verticesBuffer.ReleaseAndGetAddressOf()));

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

void Crate::Draw(_In_ ID3D11DeviceContext* context, _In_ const Camera* camera)
{
	auto renderFX = dynamic_cast<FXBasicEffect*>(m_renderFX);
	context->IASetInputLayout(renderFX->GetInputLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	auto VB = m_verticesBuffer.Get();
	

	//set constants
	
	renderFX->SetEyePosW(camera->GetPosition());
	
	renderFX->SetDirLights(mDirLights);

	context->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
	context->IASetIndexBuffer(m_indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	XMMATRIX world = XMLoadFloat4x4(&m_World);
	XMMATRIX ViewProj = camera->ViewProj();
	XMMATRIX worldViewProj = world*ViewProj;
	renderFX->SetWorldViewProjMatrix(worldViewProj);
	renderFX->SetWorld(world);
	renderFX->SetWorldInvTranspose(MathHelper::InverseTranspose(world));
	renderFX->SetMaterial(mBoxMat);
	renderFX->SetDiffuseMap(mDiffuseMapSRV.Get());
	//renderFX->SetAlphaMap(mAlphaMapSRV.Get());
	renderFX->SetTexTransform(world);
	renderFX->SetFogColor(Colors::Silver);
	renderFX->SetFogStart(15.0f);
	renderFX->SetFogRange(175.0f);

	ID3DX11EffectTechnique* activeTech = renderFX->Light1TexFog;

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		activeTech->GetPassByIndex(p)->Apply(0, context);
		context->DrawIndexed(m_Indices.size(), 0, 0);
	}
}

void Crate::Initialize(_In_ ID3D11DeviceContext* context)
{
	GeometricPrimitive::Initialize(context);
	ComPtr<ID3D11Device> device;
	context->GetDevice(&device);
	TexMetadata texMeta;
	HR(LoadFromDDSFile(L"Textures/flare.dds", DDS_FLAGS_NONE,&texMeta, m_Image));
	HR(CreateShaderResourceView(device.Get(), m_Image.GetImages(), m_Image.GetImageCount(), texMeta, mDiffuseMapSRV.ReleaseAndGetAddressOf()));
	/*HR(LoadFromDDSFile(L"Textures/flarealpha.dds", DDS_FLAGS_NONE, &texMeta, m_Image2));
	HR(CreateShaderResourceView(device.Get(), m_Image2.GetImages(), m_Image2.GetImageCount(), texMeta, mAlphaMapSRV.ReleaseAndGetAddressOf()));*/
}

void Crate::BindFX(_In_ ID3D11Device* device)
{
	m_renderFX = FXEffectsFactory::GetFXBasicEffect(device);
}
