#include "stdafx.h"
#include "GeometryGenerator.h"

void GeometryGenerator::CreateBox(float width, float height, float depth, MeshData & meshData)
{
	meshData.Vertices.resize(24);
	meshData.Indices.resize(36);

	//
	// Create the vertices.
	//

	Vertex v[24];

	float w2 = 0.5f*width;
	float h2 = 0.5f*height;
	float d2 = 0.5f*depth;

	// Fill in the front face vertex data.
	v[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	v[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	v[8] = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[9] = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	v[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	v[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	meshData.Vertices.assign(&v[0], &v[24]);

	//
	// Create the indices.
	//

	UINT i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	meshData.Indices.assign(&i[0], &i[36]);
}

void GeometryGenerator::CreateHexCell(float innerRadius, float outerRadius, MeshData & mesh,const XMFLOAT3 offset)
{
	mesh.Vertices.resize(7);
	mesh.Indices.resize(18);
	mesh.Vertices[0] = Vertex(XMFLOAT3(0.f + offset.x, 0.f+offset.y, 0.f+offset.z), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT2(0.f, 0.f));
	mesh.Vertices[1] = Vertex(XMFLOAT3(-innerRadius + offset.x, 0 + offset.y, 0.5f*outerRadius + offset.z), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(-innerRadius, 0.f, 0.5f*outerRadius), XMFLOAT2(0.f, 0.f));
	mesh.Vertices[2] = Vertex(XMFLOAT3(0 + offset.x, 0 + offset.y, outerRadius + offset.z), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(0.f, 0.f, 0.5f*outerRadius), XMFLOAT2(0.f, 0.f));
	mesh.Vertices[3] = Vertex(XMFLOAT3(innerRadius + offset.x, 0 + offset.y, 0.5f*outerRadius + offset.z), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(innerRadius, 0.f, 0.5f*outerRadius), XMFLOAT2(0.f, 0.f));
	mesh.Vertices[4] = Vertex(XMFLOAT3(innerRadius + offset.x, 0 + offset.y, -0.5f*outerRadius + offset.z), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(innerRadius, 0.f, -0.5f*outerRadius), XMFLOAT2(0.f, 0.f));
	mesh.Vertices[5] = Vertex(XMFLOAT3(0 + offset.x, 0 + offset.y, -outerRadius + offset.z), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(0, 0.f, -outerRadius), XMFLOAT2(0.f, 0.f));
	mesh.Vertices[6] = Vertex(XMFLOAT3(-innerRadius + offset.x, 0 + offset.y, -0.5f*outerRadius + offset.z), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(-innerRadius, 0.f, -0.5f*outerRadius), XMFLOAT2(0.f, 0.f));

	for (UINT i = 0;i<mesh.Indices.size()/3;i++)
	{
		mesh.Indices[i*3] = 0;
		mesh.Indices[i*3 + 1] = i + 1;
		mesh.Indices[i * 3 + 2] = (i + 2)>6?1: (i + 2);
	}
}

void GeometryGenerator::CreateHexGrid(UINT width, UINT depth, float innerRadius, float outerRadius, MeshData & mesh)
{
	mesh.Indices.clear();
	mesh.Vertices.clear();
	XMFLOAT3 Pos{0.f,0.f,0.f};
	for (UINT y = 0;y<depth;y++)
	{
		for (UINT x = 0;x<width;x++)
		{
			MeshData cellMesh;
			Pos.x = x*innerRadius * 2+ y%2*innerRadius;
			Pos.z = y*outerRadius*1.5;
			CreateHexCell(innerRadius, outerRadius, cellMesh, Pos);
			mesh.Vertices.insert(mesh.Vertices.end(), cellMesh.Vertices.begin(), cellMesh.Vertices.end());
			mesh.Indices.insert(mesh.Indices.end(), cellMesh.Indices.begin(), cellMesh.Indices.end());
		}
	}
}

