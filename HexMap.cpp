#include "stdafx.h"
#include "HexMap.h"
#include "MathHelper.h"
#include <assert.h>
#include "DirectXColors.h"
#include "ApplicationD3D11.h"

using namespace Microsoft::WRL;
//using namespace DirectX;
using namespace std;

const float IRM = 0.866025404f;

HexMap::HexMap(float outerRadiuse, UINT width, UINT height)
	:GeometricPrimitive()
	,m_width(width)
	,m_depth(height)
{
	for (UINT y=0;y<m_depth;y++)
	{
		for (UINT x = 0;x<m_width;x++)
		{
			m_hexCells.push_back(make_shared<HexCell>(x,y,outerRadiuse*IRM,outerRadiuse));
		}
	}
	for (size_t i = 0;i<m_hexCells.size();i++)
	{
		auto pos = m_hexCells[i]->GetCellCoordinates();
		if (pos.y%2==1)
		{
			m_hexCells[i]->SetNeighborCell(GetHexCell(pos.x + 1, pos.y + 1), HexCell::NE);
			m_hexCells[i]->SetNeighborCell(GetHexCell(pos.x + 1, pos.y), HexCell::E);
			m_hexCells[i]->SetNeighborCell(GetHexCell(pos.x + 1, pos.y - 1), HexCell::SE);
			m_hexCells[i]->SetNeighborCell(GetHexCell(pos.x, pos.y - 1), HexCell::SW);
			m_hexCells[i]->SetNeighborCell(GetHexCell(pos.x -1, pos.y), HexCell::W);
			m_hexCells[i]->SetNeighborCell(GetHexCell(pos.x, pos.y + 1), HexCell::NW);
		}
		else
		{
			m_hexCells[i]->SetNeighborCell(GetHexCell(pos.x, pos.y + 1), HexCell::NE);
			m_hexCells[i]->SetNeighborCell(GetHexCell(pos.x + 1, pos.y), HexCell::E);
			m_hexCells[i]->SetNeighborCell(GetHexCell(pos.x, pos.y - 1), HexCell::SE);
			m_hexCells[i]->SetNeighborCell(GetHexCell(pos.x-1, pos.y - 1), HexCell::SW);
			m_hexCells[i]->SetNeighborCell(GetHexCell(pos.x - 1, pos.y), HexCell::W);
			m_hexCells[i]->SetNeighborCell(GetHexCell(pos.x-1, pos.y + 1), HexCell::NW);
		}
	}
}


void HexMap::Initialize(ID3D11DeviceContext * context)
{
	GeometricPrimitive::Initialize(context);
	for (size_t i =0;i<m_hexCells.size();i++)
	{
		m_hexCells[i]->Initialize(context);
	}

	ComPtr<ID3D11Device> device;
	context->GetDevice(&device);
	D3D11_RASTERIZER_DESC rd{};
	rd.CullMode = D3D11_CULL_NONE;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.FrontCounterClockwise = FALSE;
	HR(device->CreateRasterizerState(&rd, m_RasterizerState.ReleaseAndGetAddressOf()));
}

void HexMap::Draw(ID3D11DeviceContext * context, const Camera * camera)
{
	context->IASetInputLayout(m_renderFX->GetInputLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->RSSetState(m_RasterizerState.Get());
	//set constants
	XMMATRIX world = XMLoadFloat4x4(&m_World);
	XMMATRIX ViewProj = camera->ViewProj();
	XMMATRIX worldViewProj = world*ViewProj;
	m_renderFX->SetWorldViewProjMatrix(worldViewProj);

	D3DX11_TECHNIQUE_DESC techDesc;
	m_renderFX->GetEffectTechnique()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_renderFX->GetEffectTechnique()->GetPassByIndex(p)->Apply(0, context);
		for (UINT i = 0;i<m_hexCells.size();i++)
		{
			m_hexCells[i]->Draw(context,nullptr);
		}
	}
	context->RSSetState(0);
}

HexCell * HexMap::GetHexCell(UINT x, UINT y) const
{
	if (x >= 0 && x < m_width&&y >= 0 && y<m_depth)
	{
		return m_hexCells[y*m_width + x].get();
	}
	return nullptr;
}

HexCell::HexCell(UINT x, UINT y, float innerRadius, float outerRadius, FXMVECTOR color)
	:m_Coordinates(x, y)
	, m_innerRadiuse(innerRadius)
	, m_outerRadiuse(outerRadius)
{
	XMStoreFloat4(&m_Color, color);
}

void HexCell::BuildGeometryBuffer(ID3D11Device * device)
{
	m_Indices.clear();
	m_Vertices.clear();
	XMFLOAT3 Pos{ 0.f,0.f,0.f };
	Pos.x = m_Coordinates.x*m_innerRadiuse * 2 + m_Coordinates.y % 2 * m_innerRadiuse;
	Pos.z = m_Coordinates.y*m_outerRadiuse*1.5;
	GeometryGenerator::MeshData mesh;
	GeometryGenerator::CreateHexCell(m_innerRadiuse, m_outerRadiuse, mesh, Pos);
	m_Indices.insert(m_Indices.end(), mesh.Indices.begin(), mesh.Indices.end());
	m_Vertices.resize(VerticesCount);
	for (UINT i = 0; i < VerticesCount; i++)
	{
		VertexTypes::ConvertFromGemometryVertex(m_Vertices[i], mesh.Vertices[i]);
		m_Vertices[i].Color = m_Color;
	}
	D3D11_BUFFER_DESC vbd;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.CPUAccessFlags = 0;
	vbd.ByteWidth = sizeof(VertexTypes::VertexPostionColor)*m_Vertices.size();
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = m_Vertices.data();

	HR(device->CreateBuffer(&vbd, &vinitData, m_verticesBuffer.ReleaseAndGetAddressOf()));

	D3D11_BUFFER_DESC ibd;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.CPUAccessFlags = 0;
	ibd.ByteWidth = sizeof(UINT)*m_Indices.size();
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = m_Indices.data();
	HR(device->CreateBuffer(&ibd, &iinitData, m_indicesBuffer.ReleaseAndGetAddressOf()));
}

void HexCell::Draw(ID3D11DeviceContext * context, const Camera * camera)
{
	UINT stride = sizeof(VertexTypes::VertexPostionColor);
	UINT offset = 0;
	auto VB = m_verticesBuffer.Get();
	context->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
	context->IASetIndexBuffer(m_indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->DrawIndexed(IndicesCount, 0, 0);
}
