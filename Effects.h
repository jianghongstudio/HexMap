#pragma once
#include "d3d11.h"
#include <wrl/client.h>
#include <mutex>
using namespace Microsoft::WRL;
class BasicEffects
{
public:
	explicit BasicEffects(_In_ ID3D11Device* device);
	virtual ~BasicEffects();

	ID3D11VertexShader* GetVertexShader();
	ID3D11PixelShader*  GetPixelShader();
private:
	
	ComPtr<ID3D11Device>	   m_Device;
	ComPtr<ID3D11VertexShader> m_VertexShader;
	ComPtr<ID3D11PixelShader>  m_PixelShader;

	std::mutex m_Mutex;
};

