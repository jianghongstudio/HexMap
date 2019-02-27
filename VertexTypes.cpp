#include "stdafx.h"
#include "VertexTypes.h"

using namespace VertexTypes;

const D3D11_INPUT_ELEMENT_DESC VertexPostionColor::InputElements[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

const D3D11_INPUT_ELEMENT_DESC VertexPostionNormalTex::InputElements[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

void VertexTypes::VertexPostionColor::ConvertFromGemometryVertex(const GeometryGenerator::Vertex & gVertex)
{
	this->Pos = gVertex.Position;
}

void VertexTypes::VertexPostionNormalTex::ConvertFromGemometryVertex(const GeometryGenerator::Vertex & gVertex)
{
	this->Pos = gVertex.Position;
	this->Normal = gVertex.Normal;
	this->Tex = gVertex.TexC;
}
