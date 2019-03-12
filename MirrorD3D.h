#pragma once
#include "RenderD3D11.h"
#include "GeometricPrimitive.h"
#include "ApplicationD3D11.h"
#include "Crate.h"
class MirrorD3D :
	public ApplicationD3D11,public GeometricPrimitive<VertexTypes::VertexPostionNormalTex>
{
public:
	MirrorD3D(_In_ const TCHAR* titleName = _T("D3D11App"), _In_ const TCHAR* className = _T("WndClass"));
	virtual ~MirrorD3D();

	virtual bool InitApp(HINSTANCE hinstance) override;

	void BindFX(ID3D11Device * device) override;

	void UpdateScene(float deltaTime) override;

	void RenderScene() override;
protected:
	
	// Inherited via GeometricPrimitive
	virtual void BuildGeometryBuffer(ID3D11Device * device) override;

	virtual void Draw(ID3D11DeviceContext * context, const Camera * camera) override;

private:
	ComPtr<ID3D11ShaderResourceView> m_floorDiffuseMapSRV;
	ComPtr<ID3D11ShaderResourceView> m_wallDiffuseMapSRV;
	ComPtr<ID3D11ShaderResourceView> m_mirrorDiffuseMapSRV;
	ComPtr<ID3D11BlendState> m_noRenderTargetDBS;
	ComPtr<ID3D11DepthStencilState> m_makeMirrorDSS;
	ComPtr<ID3D11DepthStencilState> m_reflectionDSS;
	ComPtr<ID3D11BlendState> m_transparentDBS;

	DirectionalLight mDirLights[3];
	Material mRoomMat;
	Material mMirrorMat;

	Crate m_crate;
	Crate m_reflectionCrate;

};

