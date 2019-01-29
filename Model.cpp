#include "stdafx.h"
#include "Model.h"
#include "DirectXColors.h"
#include "ApplicationD3D11.h"

const D3D11_INPUT_ELEMENT_DESC Model::InputElements[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

PrimitiveBox::PrimitiveBox(UINT width, UINT height,UINT depth)
	:Model()
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
		context->DrawIndexed(m_Indices.size(), 0, 0);
	}
}

void PrimitiveBox::BuildGeometryBuffer(ID3D11Device * device)
{
	m_Vertices.clear();
	m_Indices.clear();
	GeometryGenerator::MeshData box;
	m_geometryGenerator.CreateBox(m_width, m_height, m_depth, box);

	std::vector<Vertex> Vertices(box.Vertices.size());
	for (size_t i = 0; i < Vertices.size(); i++)
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

Model::Model()
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_World, I);
}

void Model::Initialize(ID3D11DeviceContext * context)
{
	ComPtr<ID3D11Device> device;
	context->GetDevice(&device);
	BuildFXandInputLayout(device.Get());
	BuildGeometryBuffer(device.Get());
}

void Model::BuildFXandInputLayout(ID3D11Device * device)
{
	m_FX = std::make_unique<FXEffect>(device, "fx/color.fxo");
	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	m_FX->GetEffectTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputElements, InputElementCount, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, m_InputLayout.ReleaseAndGetAddressOf()));
}
