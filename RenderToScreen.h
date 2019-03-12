#pragma once
#include "ApplicationD3D11.h"
#include "GeometricPrimitive.h"

class BlurDemo :
	public ApplicationD3D11,public GeometricPrimitive<VertexTypes::VertexPostionNormalTex>
{
public:
	BlurDemo(_In_ const TCHAR* titleName = _T("D3D11App"), _In_ const TCHAR* className = _T("WndClass"));
	virtual ~BlurDemo();

	// Inherited via GeometricPrimitive
	virtual void BuildGeometryBuffer(ID3D11Device * device) override;

	virtual void Draw(ID3D11DeviceContext * context, const Camera * camera) override;

	bool InitApp(HINSTANCE hinstance) override;

	void RenderScene() override;

protected:
	void BindFX(_In_ ID3D11Device* device) override;

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_diffuseMap;

};

