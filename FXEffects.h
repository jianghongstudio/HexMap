#pragma once
#include "d3dx11effect.h"
#include "DirectXMath.h"
#include <wrl\client.h>
#include <memory>
//#include "LightHelper.h"
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
protected:

	Microsoft::WRL::ComPtr<ID3DX11Effect> m_FX;
	ID3DX11EffectTechnique* m_pTech;
	ID3DX11EffectMatrixVariable*  m_pfxWorldViewProj;
};

//class FXEffectLighting:public FXEffect
//{
//public:
//	FXEffectLighting(_In_ ID3D11Device* device, _In_ const char* FXPath);
//	virtual ~FXEffectLighting() {};
//
//	void SetWorldMatrix(_In_ const float* value);
//	void SetWorldInvTransposeMatrix(_In_ const float* value);
//	void SetEyePos(_In_ void* data, _In_ UINT offset, _In_ size_t dataSize);
//	void SetDirLight(_In_ void* data, _In_ UINT offset, _In_ size_t dataSize);
//	void SetPointLight(_In_ void* data, _In_ UINT offset, _In_ size_t dataSize);
//	void SetSpotLight(_In_ void* data, _In_ UINT offset, _In_ size_t dataSize);
//	void SetMaterial(_In_ void* data, _In_ UINT offset, _In_ size_t dataSize);
//protected:
//	ID3DX11EffectMatrixVariable* m_pFXWorld;
//	ID3DX11EffectMatrixVariable* m_pFXWorldInvTranspose;
//	ID3DX11EffectVectorVariable* m_pFXEyePosW;
//	ID3DX11EffectVariable* m_pFXDirLight;
//	ID3DX11EffectVariable* m_pFXPointLight;
//	ID3DX11EffectVariable* m_pFXSpotLight;
//	ID3DX11EffectVariable* m_pFXMaterial;
//};

//class FXBasicEffect:public FXEffect
//{
//public:
//	FXBasicEffect(_In_ ID3D11Device* device, _In_ const char* FXPath);
//	~FXBasicEffect() {};
//	void SetWorld(CXMMATRIX M) { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
//	void SetWorldInvTranspose(CXMMATRIX M) { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
//	void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
//	void SetDirLights(const DirectionalLight* lights) { DirLights->SetRawValue(lights, 0, 3 * sizeof(DirectionalLight)); }
//	void SetMaterial(const Material& mat) { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
//
//	ID3DX11EffectTechnique* Light1Tech;
//	ID3DX11EffectTechnique* Light2Tech;
//	ID3DX11EffectTechnique* Light3Tech;
//
//	ID3DX11EffectMatrixVariable* World;
//	ID3DX11EffectMatrixVariable* WorldInvTranspose;
//	ID3DX11EffectVectorVariable* EyePosW;
//	ID3DX11EffectVariable* DirLights;
//	ID3DX11EffectVariable* Mat;
//private:
//	
//};


class  FXEffectsFactory
{

public:
	FXEffectsFactory() = delete;
	~FXEffectsFactory() {}

	template<typename T>
	static unique_ptr<T> CreateFXEffectFromCompiledFile(_In_ ID3D11Device* device, _In_ const char* FXPath)
	{
		return make_unique<T>(device, FXPath);
	}
private:

};