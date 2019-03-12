#pragma once
#include <wrl/client.h>
#include "d3d11.h"
#include "FXEffects.h"
#include "GeometryGenerator.h"
#include "Camera.h"
#include "VertexTypes.h"
using namespace Microsoft::WRL;

template <typename T>
class GeometricPrimitive
{
public:
	typedef T VertexType;
	GeometricPrimitive();
	virtual ~GeometricPrimitive() {};

	virtual void Initialize(_In_ ID3D11DeviceContext* context);
	virtual void BuildGeometryBuffer(_In_ ID3D11Device* device) = 0;
	virtual void Draw(_In_ ID3D11DeviceContext* context, _In_ const Camera* camera) = 0;
	virtual void Update(float dt) { UpdateMatrix(); }

	void SetPostion(float x, float y, float z) { m_Postion = { x,y,z }; }
	void SetAngle(float x, float y, float z) { m_Rotation = { x,y,z }; }
	void SetScaling(float x, float y, float z) { m_Scaling = { x,y,z }; }
protected:
	virtual void BindFX(_In_ ID3D11Device* device);
	void    UpdateMatrix();
protected:
	ComPtr<ID3D11Buffer> m_verticesBuffer;
	ComPtr<ID3D11Buffer> m_indicesBuffer;

	FXEffect* m_renderFX;

	XMFLOAT3 m_Postion = {0,0,0};
	XMFLOAT3 m_Rotation = { 0,0,0 };
	XMFLOAT3 m_Scaling = { 1,1,1 };
	XMFLOAT4X4 m_World;

	std::vector<UINT> m_Indices;
	std::vector<VertexType> m_Vertices;
private:
};

template <typename T>
GeometricPrimitive<T>::GeometricPrimitive()
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_World, I);
}
template <typename T>
void GeometricPrimitive<T>::Initialize(ID3D11DeviceContext * context)
{
	ComPtr<ID3D11Device> device;
	context->GetDevice(&device);
	BindFX(device.Get());
	BuildGeometryBuffer(device.Get());
}

template <typename T>
void GeometricPrimitive<T>::BindFX(ID3D11Device * device)
{
	m_renderFX = FXEffectsFactory::GetFXEffect(device);
}

template<typename T>
inline void GeometricPrimitive<T>::UpdateMatrix()
{
	auto translation = XMMatrixTranslation(m_Postion.x, m_Postion.y, m_Postion.z);
	auto rotation = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	auto scaling = XMMatrixScaling(m_Scaling.x, m_Scaling.y, m_Scaling.z);
	auto worldMatrix = translation*rotation*scaling;
	XMStoreFloat4x4(&m_World, worldMatrix);
}

class PrimitiveBox :public GeometricPrimitive<VertexTypes::VertexPostionColor>
{
public:

	PrimitiveBox(float width = 10, float height = 10, float depth = 10);
	virtual ~PrimitiveBox();

	void Draw(ID3D11DeviceContext* context, const Camera* camera);
	virtual void BuildGeometryBuffer(_In_ ID3D11Device* device);

private:
	GeometryGenerator m_geometryGenerator;

	float m_width;
	float m_height;
	float m_depth;
};

class PrimitiveRectangular: public GeometricPrimitive<VertexTypes::VertexPostionColor>
{
public:
	PrimitiveRectangular(float width = 10.f, float height = 10.f,FXMVECTOR color = Colors::Green);
	virtual ~PrimitiveRectangular() {};
	// Inherited via GeometricPrimitive
	virtual void BuildGeometryBuffer(ID3D11Device * device) override;

	virtual void Draw(ID3D11DeviceContext * context, const Camera * camera) override;
private:

	float m_width;
	float m_height;

	XMFLOAT4 m_color;
};

class Sphere: public GeometricPrimitive<VertexTypes::VertexPostionNormalTex>
{
public:
	Sphere(float radius = 5.f, UINT sliceCount = 30, UINT stackCount = 30);
	~Sphere() {}

	// Inherited via GeometricPrimitive
	virtual void BuildGeometryBuffer(ID3D11Device * device) override;

	virtual void Draw(ID3D11DeviceContext * context, const Camera * camera) override;

	void BindFX(_In_ ID3D11Device* device) override;

	void SetMaterial(const XMFLOAT4& Ambient, const XMFLOAT4& Diffuse, const XMFLOAT4& Specular, const XMFLOAT4& Reflect);
	void SetCubeMap(ID3D11ShaderResourceView* map) { cubeMap = map; }
	void SetDirLights(DirectionalLight* dirlights) { mDirLights = dirlights; }

private:
	float m_radius;
	float m_sliceCount;
	float m_stackCount;

	Material m_sphereMaterial;
	DirectionalLight* mDirLights;

	ID3D11ShaderResourceView* cubeMap;
};

