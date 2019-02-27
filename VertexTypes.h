#pragma once
#include "d3d11.h"
#include "DirectXMath.h"
#include "DirectXColors.h"
#include "GeometryGenerator.h"
using namespace DirectX;
namespace VertexTypes
{
	struct  VertexPostionColor
	{
		XMFLOAT3 Pos;
		XMFLOAT4 Color = XMFLOAT4((const float*)&Colors::Red);

		static const int InputElementCount = 2;
		static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];

		void ConvertFromGemometryVertex(const GeometryGenerator::Vertex& gVertex);
	};

	struct  VertexPostionNormalTex
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;

		static const int InputElementCount = 3;
		static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
		void ConvertFromGemometryVertex(const GeometryGenerator::Vertex& gVertex);
	};

	template<typename T>
	void ConvertFromGemometryVertex(T& vertex, const GeometryGenerator::Vertex& gVertex)
	{
		vertex.ConvertFromGemometryVertex(gVertex);
	}
}