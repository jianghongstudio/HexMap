#pragma once
#include "d3dx11effect.h"
#include <wrl\client.h>
#include <memory>
#include "LightHelper.h"
#include "VertexTypes.h"
using namespace std;
using namespace DirectX;
class FXEffect
{
public:
	FXEffect(_In_ ID3D11Device* device, _In_ const char* FXPath);
	virtual ~FXEffect();

	void SetWorldViewProjMatrix(CXMMATRIX M) { m_pfxWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void Apply(_In_ UINT index, _In_ UINT flag, _In_ ID3D11DeviceContext* context);
	ID3DX11EffectTechnique* GetEffectTechnique() const { return m_pTech; }

	virtual ID3D11InputLayout* GetInputLayout();
protected:
	Microsoft::WRL::ComPtr<ID3D11Device>   m_Device;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
	Microsoft::WRL::ComPtr<ID3DX11Effect> m_FX;
	ID3DX11EffectTechnique* m_pTech;
	ID3DX11EffectMatrixVariable*  m_pfxWorldViewProj;
};

class FXBasicEffect :public FXEffect
{
public:
	FXBasicEffect(_In_ ID3D11Device* device, _In_ const char* FXPath);
	~FXBasicEffect() {};
	void SetWorld(CXMMATRIX M) { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M) { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetDirLights(const DirectionalLight* lights) { DirLights->SetRawValue(lights, 0, 3 * sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat) { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex) { DiffuseMap->SetResource(tex); }
	void SetAlphaMap(ID3D11ShaderResourceView* tex) { AlphaMap->SetResource(tex); }
	void SetTexTransform(CXMMATRIX M) { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetFogColor(const FXMVECTOR v) { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f) { FogStart->SetFloat(f); }
	void SetFogRange(float f) { FogRange->SetFloat(f); }

	ID3D11InputLayout* GetInputLayout() override;


	ID3DX11EffectTechnique* Light1Tech;
	ID3DX11EffectTechnique* Light2Tech;
	ID3DX11EffectTechnique* Light3Tech;

	ID3DX11EffectTechnique* Light2Tex;
	ID3DX11EffectTechnique* Light1TexFog;

	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* AlphaMap;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;

private:

};


class  FXEffectsFactory
{

public:
	FXEffectsFactory() = delete;
	~FXEffectsFactory() {}

	static FXEffect* GetFXEffect(_In_ ID3D11Device* device);
	static FXBasicEffect* GetFXBasicEffect(_In_ ID3D11Device* device);
private:
	template<typename T>
	static T* CreateFXEffectFromCompiledFile(_Inout_ unique_ptr<T>& effect, _In_ ID3D11Device* device, _In_ const char* FXPath)
	{
		if (!effect.get())
		{
			effect = make_unique<T>(device, FXPath);
		}
		return effect.get();
	}

	static unique_ptr<FXBasicEffect> s_FXBasicEffect;
	static unique_ptr<FXEffect> s_FXEffect;
};