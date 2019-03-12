#pragma once
#include "stdafx.h"
#include <vector>
#include "DirectXMath.h"
using namespace DirectX;
class GeometryGenerator
{
public:
	struct Vertex
	{
		Vertex() {}
		Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv)
			: Position(p), Normal(n), TangentU(t), TexC(uv) {}
		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			: Position(px, py, pz), Normal(nx, ny, nz),
			TangentU(tx, ty, tz), TexC(u, v) {}

		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT3 TangentU;
		XMFLOAT2 TexC;
	};

	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<UINT> Indices;
	};

	///<summary>
	/// Creates a box centered at the origin with the given dimensions.
	///</summary>
	static void CreateBox(float width, float height, float depth, MeshData& meshData);

	static void CreateHexCell(float innerRadius, float outerradius, MeshData& mesh, const XMFLOAT3 offset={0.f,0.f,0.f});

	static void CreateHexGrid(UINT width, UINT depth, float innerRadius, float outerRadius, MeshData& mesh);

	static void CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& meshData);
};

