#pragma once
#include <wrl\client.h>
#include "GeometricPrimitive.h"
#include "DirectXColors.h"

class HexCell:public GeometricPrimitive<VertexTypes::VertexPostionColor>
{
public:
	struct HexCoordinates
	{
		HexCoordinates(UINT cx = 0, UINT cy = 0) :x(cx), y(cy) {}
		UINT x = 0;
		UINT y = 0;
	};
	enum HexCellDirections:UINT
	{
		NE = 0,
		E,
		SE,
		SW,
		W,
		NW
	};

	HexCell(UINT x, UINT y, float innerRadius, float outerRadius, FXMVECTOR color = DirectX::Colors::Green);
	virtual ~HexCell() {}

	void SetNeighborCell(_In_ HexCell* neighborCell, HexCellDirections dirction) { m_neighbors[dirction] = neighborCell; }
	HexCell* GetNeighborCell(HexCellDirections dirction) const { return m_neighbors[dirction]; }

	void SetColor(FXMVECTOR& color) { XMStoreFloat4(&m_Color, color); }
	const XMFLOAT4& GetColor() const { return m_Color; }

	virtual void BuildGeometryBuffer(_In_ ID3D11Device * device) override;
	virtual void Draw(_In_ ID3D11DeviceContext * context, _In_ const Camera * camera) override;

	const HexCoordinates& GetCellCoordinates() const { return m_Coordinates; }

	static const int VerticesCount = 7;
	static const int IndicesCount = 18;
private:
	XMFLOAT4 m_Color;

	float m_innerRadiuse;
	float m_outerRadiuse;

	HexCoordinates m_Coordinates;
	HexCell* m_neighbors[6];


	// Inherited via Model
	

};

class HexMap:public GeometricPrimitive<VertexTypes::VertexPostionColor>
{
public: 
	HexMap(float outerRadiuse, UINT width, UINT height);
	virtual ~HexMap(){}
	// Inherited via Model
	virtual void Initialize(_In_ ID3D11DeviceContext* context) override;
	virtual void Draw(_In_ ID3D11DeviceContext * context, _In_ const Camera * camera) override;
	HexCell* GetHexCell(UINT x, UINT y) const;

private:
	virtual void BuildGeometryBuffer(_In_ ID3D11Device * device) override {};
	UINT m_width;
	UINT m_depth;
	ComPtr<ID3D11RasterizerState> m_RasterizerState;


	vector<shared_ptr<HexCell>> m_hexCells;
};

