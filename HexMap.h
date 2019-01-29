#pragma once
#include <wrl\client.h>
#include "Model.h"

class HexMap:public Model
{
public: 
	HexMap(float innerRadiuse, float outerRadiuse, UINT width, UINT height);
	virtual ~HexMap(){}
	// Inherited via Model
	virtual void Draw(ID3D11DeviceContext * context, const Camera * camera) override;
	virtual void BuildGeometryBuffer(ID3D11Device * device) override;
private:

	GeometryGenerator m_geometryGenerator;

	float m_innerRadiuse;
	float m_outerRadiuse;

	UINT m_width;
	UINT m_depth;
};

