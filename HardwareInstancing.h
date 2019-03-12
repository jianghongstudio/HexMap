#pragma once
#include "ApplicationD3D11.h"
#include "Crate.h"
#include "Sky.h"
class HardwareInstancing :
	public ApplicationD3D11,public Crate
{
public:
	HardwareInstancing(_In_ const TCHAR* titleName = _T("D3D11App"), _In_ const TCHAR* className = _T("WndClass"))
		:ApplicationD3D11(titleName, className)
		, Crate(5, 5, 5)
	{

	}
	virtual ~HardwareInstancing() {}

	struct InstanceData
	{
		InstanceData() {}
		InstanceData(XMFLOAT4X4 a_world,XMFLOAT4 a_color):world(a_world),color(a_color){}
		XMFLOAT4X4 world;
		XMFLOAT4 color;
	};

	bool InitApp(HINSTANCE hinstance) override;
	void BindFX(_In_ ID3D11Device* device) override;
	void RenderScene() override;

	void Draw(_In_ ID3D11DeviceContext* context, _In_ const Camera* camera) override;

	void BuildGeometryBuffer(_In_ ID3D11Device* device) override;

	void UpdateScene(float deltaTime) override;

	void OnMouseButtonDown(WPARAM btnState, int x, int y) override;

	void GetRayInViewSpace(_In_ int sx, _In_ int sy, _In_ const Camera& cam, _Out_opt_ XMVECTOR* origin, _Out_opt_ XMVECTOR* direction);

	void DoIntersectTest(FXMVECTOR origin, FXMVECTOR direction, const Camera& cam);
protected:
	vector<InstanceData> m_Instancies;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_InstanceBuffer;

	unique_ptr<Sky> m_sky;

	Sphere m_sphere;
};

