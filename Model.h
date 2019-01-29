#pragma once
#include <wrl/client.h>
#include "d3d11.h"
#include "FXEffects.h"
#include "GeometryGenerator.h"
#include "Camera.h"
using namespace Microsoft::WRL;

class Model
{
public:

	struct  Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
	};
	static const int InputElementCount = 2;
	static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];

	Model();
	virtual ~Model() {};

	virtual void Initialize(ID3D11DeviceContext* context);
	virtual void BuildGeometryBuffer(ID3D11Device* device) = 0;
	virtual void Draw(ID3D11DeviceContext* context, const Camera* camera) = 0;
	virtual void Update(float dt) {};
protected:
	virtual void BuildFXandInputLayout(ID3D11Device* device);
protected:
	ComPtr<ID3D11Buffer> m_verticesBuffer;
	ComPtr<ID3D11Buffer> m_indicesBuffer;

	std::unique_ptr<FXEffect> m_FX;

	ComPtr<ID3D11InputLayout> m_InputLayout;

	XMFLOAT4X4 m_World;

	std::vector<UINT> m_Indices;
	std::vector<Vertex> m_Vertices;
private:

};


class PrimitiveBox:public Model
{
public:

	PrimitiveBox(UINT width = 10,UINT height = 10,UINT depth = 10);
	virtual ~PrimitiveBox();

	void Draw(ID3D11DeviceContext* context,const Camera* camera);
	virtual void BuildGeometryBuffer(ID3D11Device* device);
	
private:
	GeometryGenerator m_geometryGenerator;
	
	UINT m_width;
	UINT m_height;
	UINT m_depth;
};

