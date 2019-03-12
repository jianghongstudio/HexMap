#pragma once
#include "GeometricPrimitive.h"
#include "DirectXTex.h"
#include "MathHelper.h"

class Crate:public GeometricPrimitive<VertexTypes::VertexPostionNormalTex>
{
public:
	Crate(float width = 10, float height = 10, float depth = 10);
	virtual ~Crate();

	virtual void BuildGeometryBuffer(_In_ ID3D11Device* device) override;


	virtual void Draw(_In_ ID3D11DeviceContext* context, _In_ const Camera* camera) override;


	virtual void Initialize(_In_ ID3D11DeviceContext* context) override;

protected:
	virtual void BindFX(_In_ ID3D11Device* device) override;

	float m_width;
	float m_height;
	float m_depth;
	DirectX::ScratchImage m_Image;
	DirectX::ScratchImage m_Image2;
	ComPtr<ID3D11ShaderResourceView> mDiffuseMapSRV;
	ComPtr<ID3D11ShaderResourceView> mAlphaMapSRV;

	DirectionalLight mDirLights[3];
	Material mBoxMat;

	XMFLOAT4X4 mTexTransform;
};
