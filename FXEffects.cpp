#include "stdafx.h"
#include "FXEffects.h"
#include <iostream>
#include <fstream>
#include <assert.h>
#include <vector>


FXEffect::~FXEffect()
{
	
}

void FXEffect::Apply(UINT index, UINT flag, ID3D11DeviceContext * context)
{
	m_pTech->GetPassByIndex(index)->Apply(0, context);
}

FXEffect::FXEffect(ID3D11Device * device, const char * FXPath)
	:m_pTech(nullptr)
	,m_pfxWorldViewProj(nullptr)
{
	assert(device);
	assert(FXPath);
	assert(strstr(FXPath, ".fxo") != NULL);
	std::ifstream fin(FXPath, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size,
		0, device, m_FX.ReleaseAndGetAddressOf()));
	m_pTech = m_FX->GetTechniqueByIndex(0);
	m_pfxWorldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

//FXEffectLighting::FXEffectLighting(ID3D11Device * device, const char * FXPath):FXEffect(device,FXPath)
//{
//	m_pFXWorld = m_FX->GetVariableByName("gWorld")->AsMatrix();
//	m_pFXWorldInvTranspose = m_FX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
//	m_pFXEyePosW = m_FX->GetVariableByName("gEyePosW")->AsVector();
//	m_pFXDirLight = m_FX->GetVariableByName("gDirLight");
//	m_pFXPointLight = m_FX->GetVariableByName("gPointLight");
//	m_pFXSpotLight = m_FX->GetVariableByName("gSpotLight");
//	m_pFXMaterial = m_FX->GetVariableByName("gMaterial");
//}
//
//void FXEffectLighting::SetWorldInvTransposeMatrix(const float * value)
//{
//	m_pFXWorldInvTranspose->SetMatrix(value);
//}
//
//void FXEffectLighting::SetEyePos(void * data, UINT offset, size_t dataSize)
//{
//	m_pFXEyePosW->SetRawValue(data, offset, dataSize);
//}
//
//void FXEffectLighting::SetDirLight(void * data, UINT offset, size_t dataSize)
//{
//	m_pFXDirLight->SetRawValue(data, offset, dataSize);
//}
//
//void FXEffectLighting::SetPointLight(void * data, UINT offset, size_t dataSize)
//{
//	m_pFXPointLight->SetRawValue(data, offset, dataSize);
//}
//
//void FXEffectLighting::SetSpotLight(void * data, UINT offset, size_t dataSize)
//{
//	m_pFXSpotLight->SetRawValue(data, offset, dataSize);
//}
//
//void FXEffectLighting::SetMaterial(void * data, UINT offset, size_t dataSize)
//{
//	m_pFXMaterial->SetRawValue(data, offset, dataSize);
//}

//FXBasicEffect::FXBasicEffect(_In_ ID3D11Device* device, _In_ const char* FXPath):FXEffect(device,FXPath)
//{
//	Light1Tech = m_FX->GetTechniqueByName("Light1");
//	Light2Tech = m_FX->GetTechniqueByName("Light2");
//	Light3Tech = m_FX->GetTechniqueByName("Light3");
//	World = m_FX->GetVariableByName("gWorld")->AsMatrix();
//	WorldInvTranspose = m_FX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
//	EyePosW = m_FX->GetVariableByName("gEyePosW")->AsVector();
//	DirLights = m_FX->GetVariableByName("gDirLights");
//	Mat = m_FX->GetVariableByName("gMaterial");
//}
