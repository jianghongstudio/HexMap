#pragma once
#include "GeometricPrimitive.h"
#include "DirectXTex.h"
class Sky:public GeometricPrimitive<VertexTypes::VertexPostionNormalTex>
{
public:
	Sky(const WCHAR* skyImageFileName,float skySphereRadius);
	virtual ~Sky();

	// Inherited via GeometricPrimitive
	virtual void BuildGeometryBuffer(ID3D11Device * device) override;
	virtual void Draw(ID3D11DeviceContext * context, const Camera * camera) override;

	void BindFX(ID3D11Device * device) override;

	void Initialize(_In_ ID3D11DeviceContext* context) override;

	ID3D11ShaderResourceView* GetCubeMap() { return m_cubeMapSRV.Get(); }

private:

	float m_Radius;
	
	DirectX::ScratchImage m_SkyTexture;
	DirectX::TexMetadata m_SkyTexMeta;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubeMapSRV;
};

