#include "stdafx.h"
#include "Effects.h"
#include "FX/colorVS.h"
#include "FX/colorPS.h"

BasicEffects::BasicEffects(ID3D11Device * device)
	:m_Device(device)
{
}

BasicEffects::~BasicEffects()
{
}

ID3D11VertexShader * BasicEffects::GetVertexShader()
{
	ID3D11VertexShader* result = m_VertexShader.Get();
	if (!result)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		result = m_VertexShader.Get();
		if (!result)
		{
			HR(m_Device->CreateVertexShader(g_VS, sizeof(g_VS), nullptr, &result));
			m_VertexShader.Attach(result);
		}
	}
	return result;
}

ID3D11PixelShader * BasicEffects::GetPixelShader()
{
	ID3D11PixelShader* result = m_PixelShader.Get();
	if (!result)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		result = m_PixelShader.Get();
		if (!result)
		{
			HR(m_Device->CreatePixelShader(g_PS, sizeof(g_PS), nullptr, &result));
			m_PixelShader.Attach(result);
		}
	}
	return result;
}
