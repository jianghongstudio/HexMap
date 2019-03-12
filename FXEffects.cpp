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
}

FXBasicEffect::FXBasicEffect(_In_ ID3D11Device* device, _In_ const char* FXPath) :FXEffect(device, FXPath)
{
	Light1Tech = m_FX->GetTechniqueByName("Light1");
	Light2Tech = m_FX->GetTechniqueByName("Light2");
	Light3Tech = m_FX->GetTechniqueByName("Light3");
	Light2Tex = m_FX->GetTechniqueByName("Light2Tex");
	Light2TexFog = m_FX->GetTechniqueByName("Light2TexFog");
	Light0Tex = m_FX->GetTechniqueByName("Light0Tex");
	Light2ReflectTech = m_FX->GetTechniqueByName("Light2Reflect");
	World = m_FX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = m_FX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	m_pfxWorldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
	EyePosW = m_FX->GetVariableByName("gEyePosW")->AsVector();
	DirLights = m_FX->GetVariableByName("gDirLights");
	Mat = m_FX->GetVariableByName("gMaterial");
	DiffuseMap = m_FX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	CubeMap = m_FX->GetVariableByName("gCubeMap")->AsShaderResource();
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

unique_ptr<FXSkyEffect> FXEffectsFactory::s_FXSkyEffect;
unique_ptr<FXInstanceEffect> FXEffectsFactory::s_FXInstanceEffect;
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

FXInstanceEffect * FXEffectsFactory::GetFXInstanceEffect(ID3D11Device * device)
{
	return CreateFXEffectFromCompiledFile<FXInstanceEffect>(s_FXInstanceEffect, device, "fx/Instancing.fxo");
}

FXSkyEffect * FXEffectsFactory::GetFXSkyEffect(ID3D11Device * device)
{
	return CreateFXEffectFromCompiledFile<FXSkyEffect>(s_FXSkyEffect, device, "fx/Sky.fxo");
}

FXInstanceEffect::FXInstanceEffect(ID3D11Device * device, const char * FXPath) :FXEffect(device, FXPath)
{
	m_ViewProj = m_FX->GetVariableByName("gViewProj")->AsMatrix();
	EyePosW = m_FX->GetVariableByName("gEyePosW")->AsVector();
	DirLights = m_FX->GetVariableByName("gDirLights");
	Mat = m_FX->GetVariableByName("gMaterial");
	DiffuseMap = m_FX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	TexTransform = m_FX->GetVariableByName("gTexTransform")->AsMatrix();
	FogColor = m_FX->GetVariableByName("gFogColor")->AsVector();
	FogStart = m_FX->GetVariableByName("gFogStart")->AsScalar();
	FogRange = m_FX->GetVariableByName("gFogRange")->AsScalar();
	Light2Tex = m_FX->GetTechniqueByName("Light2Tex");
}

ID3D11InputLayout * FXInstanceEffect::GetInputLayout()
{
	auto result = m_InputLayout.Get();
	if (!result)
	{
		D3DX11_PASS_DESC passDesc;
		this->GetEffectTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
		D3D11_INPUT_ELEMENT_DESC inputElements[8] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
		};
		HR(m_Device->CreateInputLayout(inputElements, 8, passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize, &result));
		m_InputLayout.Attach(result);
	}
	return result;
}

ID3D11InputLayout * FXSkyEffect::GetInputLayout()
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
