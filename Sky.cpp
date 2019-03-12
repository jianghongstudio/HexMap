#include "stdafx.h"
#include "Sky.h"

Sky::Sky(const WCHAR * skyImageFileName, float skySphereRadius):m_Radius(skySphereRadius)
{
	assert(skyImageFileName);
	HR(LoadFromDDSFile(skyImageFileName, DDS_FLAGS_NONE, &m_SkyTexMeta, m_SkyTexture));
}

Sky::~Sky()
{
}

void Sky::BuildGeometryBuffer(ID3D11Device * device)
{
	m_Indices.clear();
	m_Vertices.clear();
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::CreateSphere(m_Radius, 30, 30, sphere);
	m_Indices.insert(m_Indices.end(), sphere.Indices.begin(), sphere.Indices.end());
	m_Vertices.resize(sphere.Vertices.size());
	for (UINT i = 0; i < m_Vertices.size(); i++)
	{
		VertexTypes::ConvertFromGemometryVertex(m_Vertices[i], sphere.Vertices[i]);
	}

	D3D11_BUFFER_DESC vbd;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.ByteWidth = sizeof(VertexType)*m_Vertices.size();
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.CPUAccessFlags = 0;
	vbd.StructureByteStride = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = m_Vertices.data();

	HR(device->CreateBuffer(&vbd, &vinitData, &m_verticesBuffer));

	D3D11_BUFFER_DESC ibd;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.CPUAccessFlags = 0;
	ibd.ByteWidth = sizeof(UINT)*m_Indices.size();
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = m_Indices.data();
	HR(device->CreateBuffer(&ibd, &iinitData, &m_indicesBuffer));
}

void Sky::Draw(ID3D11DeviceContext * context, const Camera * camera)
{
	context->IASetInputLayout(m_renderFX->GetInputLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto renderFX = dynamic_cast<FXSkyEffect*>(m_renderFX);

	auto camPos = camera->GetPosition();
	auto world = XMMatrixTranslation(camPos.x, camPos.y, camPos.z);
	auto WVP = world*camera->ViewProj();

	renderFX->SetWorldViewProjMatrix(WVP);
	renderFX->SetCubeMap(m_cubeMapSRV.Get());
	auto activeTech = renderFX->GetEffectTechnique();

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		activeTech->GetPassByIndex(p)->Apply(0, context);
		context->DrawIndexed(m_Indices.size(), 0, 0);
	}
}

void Sky::BindFX(ID3D11Device * device)
{
	m_renderFX = FXEffectsFactory::GetFXSkyEffect(device);
}

void Sky::Initialize(ID3D11DeviceContext * context)
{
	GeometricPrimitive<VertexType>::Initialize(context);
	ComPtr<ID3D11Device> device;
	context->GetDevice(&device);
	HR(CreateShaderResourceView(device.Get(), m_SkyTexture.GetImages(), m_SkyTexture.GetImageCount(), m_SkyTexture.GetMetadata(), &m_cubeMapSRV));
}
