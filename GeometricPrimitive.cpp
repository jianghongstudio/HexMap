#include "stdafx.h"
#include "GeometricPrimitive.h"
#include "DirectXColors.h"
#include "ApplicationD3D11.h"
#include "MathHelper.h"

PrimitiveBox::PrimitiveBox(float width, float height, float depth)
	:GeometricPrimitive()
	,m_width(width)
	,m_height(height)
	,m_depth(depth)
{
	
}


PrimitiveBox::~PrimitiveBox()
{
}

void PrimitiveBox::Draw(ID3D11DeviceContext * context,const Camera* camera)
{
	context->IASetInputLayout(m_renderFX->GetInputLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	auto VB = m_verticesBuffer.Get();
	context->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
	context->IASetIndexBuffer(m_indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//set constants
	XMMATRIX world = XMLoadFloat4x4(&m_World);
	XMMATRIX ViewProj = camera->ViewProj();
	XMMATRIX worldViewProj = world*ViewProj;
	//m_renderFX->SetWorldViewProjMatrix(worldViewProj);

	D3DX11_TECHNIQUE_DESC techDesc;
	m_renderFX->GetEffectTechnique()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_renderFX->GetEffectTechnique()->GetPassByIndex(p)->Apply(0, context);
		context->DrawIndexed(m_Indices.size(), 0, 0);
	}
}

void PrimitiveBox::BuildGeometryBuffer(ID3D11Device * device)
{
	m_Vertices.clear();
	m_Indices.clear();
	GeometryGenerator::MeshData box;
	m_geometryGenerator.CreateBox(m_width, m_height, m_depth, box);

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

PrimitiveRectangular::PrimitiveRectangular(float width, float height, FXMVECTOR color)
	: m_width(width)
	, m_height(height)
{
	XMStoreFloat4(&m_color, color);
}

void PrimitiveRectangular::BuildGeometryBuffer(ID3D11Device * device)
{
	m_Vertices.resize(4);
	m_Vertices[0].Pos = XMFLOAT3(-m_width / 2, m_height / 2, 0);
	m_Vertices[0].Color = m_color;
	m_Vertices[1].Pos = XMFLOAT3(m_width / 2, m_height / 2, 0);
	m_Vertices[1].Color = m_color;
	m_Vertices[2].Pos = XMFLOAT3(m_width / 2, -m_height / 2, 0);
	m_Vertices[2].Color = m_color;
	m_Vertices[3].Pos = XMFLOAT3(-m_width / 2, -m_height / 2, 0);
	m_Vertices[3].Color = m_color;
	m_Indices = { 0,1,2,0,2,3 };
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

void PrimitiveRectangular::Draw(ID3D11DeviceContext * context, const Camera * camera)
{
	context->IASetInputLayout(m_renderFX->GetInputLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	auto VB = m_verticesBuffer.Get();
	context->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
	context->IASetIndexBuffer(m_indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//set constants
	XMMATRIX world = XMLoadFloat4x4(&m_World);
	XMMATRIX ViewProj = camera->ViewProj();
	XMMATRIX worldViewProj = world*ViewProj;
	//m_renderFX->SetWorldViewProjMatrix(worldViewProj);

	D3DX11_TECHNIQUE_DESC techDesc;
	m_renderFX->GetEffectTechnique()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_renderFX->GetEffectTechnique()->GetPassByIndex(p)->Apply(0, context);
		context->DrawIndexed(m_Indices.size(), 0, 0);
	}
}

Sphere::Sphere(float radius, UINT sliceCount, UINT stackCount)
	:m_radius(radius)
	,m_sliceCount(sliceCount)
	,m_stackCount(stackCount)
	,cubeMap(nullptr)
{
	m_sphereMaterial.Ambient = XMFLOAT4(0.2f, 0.3f, 0.4f, 1.0f);
	m_sphereMaterial.Diffuse = XMFLOAT4(0.2f, 0.3f, 0.4f, 1.0f);
	m_sphereMaterial.Specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f);
	m_sphereMaterial.Reflect = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
}

void Sphere::BuildGeometryBuffer(ID3D11Device * device)
{
	m_Vertices.clear();
	m_Indices.clear();
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::CreateSphere(m_radius, m_sliceCount, m_stackCount, sphere);

	m_Vertices.resize(sphere.Vertices.size());
	for (UINT i = 0;i<m_Vertices.size();i++)
	{
		ConvertFromGemometryVertex(m_Vertices[i], sphere.Vertices[i]);
	}

	m_Indices.insert(m_Indices.end(), sphere.Indices.begin(), sphere.Indices.end());

	D3D11_BUFFER_DESC vbd;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.ByteWidth = sizeof(VertexType)*m_Vertices.size();
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = m_Vertices.data();

	HR(device->CreateBuffer(&vbd, &vinitData, &m_verticesBuffer));

	D3D11_BUFFER_DESC ibd;

	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.ByteWidth = sizeof(UINT)*m_Indices.size();
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = m_Indices.data();

	HR(device->CreateBuffer(&ibd, &iinitData, &m_indicesBuffer));
}

void Sphere::Draw(ID3D11DeviceContext * context, const Camera * camera)
{
	context->IASetInputLayout(m_renderFX->GetInputLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	auto VB = m_verticesBuffer.Get();

	context->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
	context->IASetIndexBuffer(m_indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	auto renderFX = dynamic_cast<FXBasicEffect*>(m_renderFX);

	auto world = XMLoadFloat4x4(&m_World);
	auto worldInvTranspose = MathHelper::InverseTranspose(world);
	auto worldViewProj = world* camera->ViewProj();

	renderFX->SetWorld(world);
	renderFX->SetWorldInvTranspose(worldInvTranspose);
	renderFX->SetWorldViewProjMatrix(worldViewProj);
	renderFX->SetMaterial(m_sphereMaterial);
	renderFX->SetEyePosW(camera->GetPosition());

	ID3DX11EffectTechnique* activeTech = renderFX->Light2Tech;
	if (cubeMap)
	{
		renderFX->SetCubeMap(cubeMap);
		activeTech = renderFX->Light2ReflectTech;
	}

	renderFX->SetDirLights(mDirLights);

	D3DX11_TECHNIQUE_DESC desc;
	activeTech->GetDesc(&desc);
	for (UINT i=0;i<desc.Passes;i++)
	{
		auto pass = activeTech->GetPassByIndex(i);
		pass->Apply(0,context);
		context->DrawIndexed(m_Indices.size(), 0, 0);
	}

}

void Sphere::BindFX(ID3D11Device * device)
{
	m_renderFX = FXEffectsFactory::GetFXBasicEffect(device);
}

void Sphere::SetMaterial(const XMFLOAT4 & Ambient, const XMFLOAT4 & Diffuse, const XMFLOAT4 & Specular, const XMFLOAT4 & Reflect)
{
	m_sphereMaterial.Ambient = Ambient;
	m_sphereMaterial.Diffuse = Diffuse;
	m_sphereMaterial.Specular = Specular;
	m_sphereMaterial.Reflect = Reflect;
}
