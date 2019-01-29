#include "stdafx.h"
#include "HexMap.h"
#include "MathHelper.h"
#include <assert.h>
#include "DirectXColors.h"
#include "ApplicationD3D11.h"

using namespace Microsoft::WRL;
//using namespace DirectX;
using namespace std;

HexMap::HexMap(float innerRadiuse, float outerRadiuse, UINT width, UINT height)
	:Model()
	,m_innerRadiuse(innerRadiuse)
	,m_outerRadiuse(outerRadiuse)
	,m_width(width)
	,m_depth(height)
{
	
}


void HexMap::Draw(ID3D11DeviceContext * context, const Camera * camera)
{
	context->IASetInputLayout(m_InputLayout.Get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	auto VB = m_verticesBuffer.Get();
	context->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
	context->IASetIndexBuffer(m_indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//set constants
	XMMATRIX world = XMLoadFloat4x4(&m_World);
	XMMATRIX ViewProj = camera->ViewProj();
	XMMATRIX worldViewProj = world*ViewProj;
	m_FX->SetWorldViewProjMatrix(worldViewProj);

	D3DX11_TECHNIQUE_DESC techDesc;
	m_FX->GetEffectTechnique()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_FX->GetEffectTechnique()->GetPassByIndex(p)->Apply(0, context);
		UINT indicesOffset = 0;
		UINT verticesOffset = 0;
		for (UINT i = 0;i<m_width*m_depth;i++)
		{
			context->DrawIndexed(18, indicesOffset, verticesOffset);
			indicesOffset += 18;
			verticesOffset += 7;
		}
		
	}
}

void HexMap::BuildGeometryBuffer(ID3D11Device * device)
{
	m_Vertices.clear();
	m_Indices.clear();
	GeometryGenerator::MeshData hexgrid;
	m_geometryGenerator.CreateHexGrid(m_width, m_depth, m_innerRadiuse, m_outerRadiuse,hexgrid);

	std::vector<Vertex> Vertices(hexgrid.Vertices.size());
	for (size_t i = 0; i < Vertices.size(); i++)
	{
		Vertices[i].Pos = hexgrid.Vertices[i].Position;
		Vertices[i].Color = XMFLOAT4((const float*)&Colors::Red);
	}


	m_Vertices.insert(m_Vertices.end(), Vertices.begin(), Vertices.end());
	m_Indices.insert(m_Indices.end(), hexgrid.Indices.begin(), hexgrid.Indices.end());
	D3D11_BUFFER_DESC vbd;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.CPUAccessFlags = 0;
	vbd.ByteWidth = sizeof(Vertex)*m_Vertices.size();
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
