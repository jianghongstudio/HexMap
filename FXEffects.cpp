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

ID3D11InputLayout * FXEffect::GetInputLayout()
{
	auto result = m_InputLayout.Get();
	if (!result)
	{
		D3DX11_PASS_DESC passDesc;
		this->GetEffectTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
		HR(m_Device->CreateInputLayout(VertexTypes::VertexPostionColor::InputElements, VertexTypes::VertexPostionColor::InputElementCount, passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize,&result));
		m_InputLayout.Attach(result);
	}
	return result;
}

FXEffect::FXEffect(ID3D11Device * device, const char * FXPath)
	:m_pTech(nullptr)
	,m_pfxWorldViewProj(nullptr)
	,m_Device(device)
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

FXBasicEffect::FXBasicEffect(_In_ ID3D11Device* device, _In_ const char* FXPath) :FXEffect(device, FXPath)
{
	Light1Tech = m_FX->GetTechniqueByName("Light1");
	Light2Tech = m_FX->GetTechniqueByName("Light2");
	Light3Tech = m_FX->GetTechniqueByName("Light3");
	Light2Tex = m_FX->GetTechniqueByName("Light2Tex");
	Light1TexFog = m_FX->GetTechniqueByName("Light2TexFog");
	World = m_FX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = m_FX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	EyePosW = m_FX->GetVariableByName("gEyePosW")->AsVector();
	DirLights = m_FX->GetVariableByName("gDirLights");
	Mat = m_FX->GetVariableByName("gMaterial");
	DiffuseMap = m_FX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	//AlphaMap = m_FX->GetVariableByName("gAlphaMap")->AsShaderResource();
	TexTransform = m_FX->GetVariableByName("gTexTransform")->AsMatrix();
	FogColor = m_FX->GetVariableByName("gFogColor")->AsVector();
	FogStart = m_FX->GetVariableByName("gFogStart")->AsScalar();
	FogRange = m_FX->GetVariableByName("gFogRange")->AsScalar();
}

ID3D11InputLayout * FXBasicEffect::GetInputLayout()
{
	auto result = m_InputLayout.Get();
	if (!result)
	{
		D3DX11_PASS_DESC passDesc;
		this->GetEffectTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
		HR(m_Device->CreateInputLayout(VertexTypes::VertexPostionNormalTex::InputElements, VertexTypes::VertexPostionNormalTex::InputElementCount, passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize, &result));
		m_InputLayout.Attach(result);
	}
	return result;
}

unique_ptr<FXBasicEffect> FXEffectsFactory::s_FXBasicEffect;
unique_ptr<FXEffect> FXEffectsFactory::s_FXEffect;

FXEffect * FXEffectsFactory::GetFXEffect(ID3D11Device * device)
{
	return CreateFXEffectFromCompiledFile<FXEffect>(s_FXEffect,device, "fx/color.fxo");
}

FXBasicEffect * FXEffectsFactory::GetFXBasicEffect(ID3D11Device * device)
{
	return CreateFXEffectFromCompiledFile<FXBasicEffect>(s_FXBasicEffect, device, "fx/Basic.fxo");
}
