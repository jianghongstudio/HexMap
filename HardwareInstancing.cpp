#include "stdafx.h"
#include "HardwareInstancing.h"
#include "DirectXMath.h"
#include "DirectXCollision.h"

bool HardwareInstancing::InitApp(HINSTANCE hinstance)
{
	if (!ApplicationD3D11::InitApp(hinstance))
	{
		return false;
	}

	this->Initialize(m_render->GetD3DDeviceContext());
	m_sky = make_unique<Sky>(L"Textures/grasscube1024.dds", 5000.0f);
	m_sky->Initialize(m_render->GetD3DDeviceContext());
	m_sphere.Initialize(m_render->GetD3DDeviceContext());
	m_sphere.SetCubeMap(m_sky->GetCubeMap());
	return true;
}

void HardwareInstancing::BindFX(ID3D11Device * device)
{
	m_renderFX = FXEffectsFactory::GetFXInstanceEffect(device);
}

void HardwareInstancing::RenderScene()
{
	m_render->Clear();
	this->Draw(m_render->GetD3DDeviceContext(), &m_mainCamera);
	m_render->GetD3DDeviceContext()->RSSetState(0);
	m_sphere.Draw(m_render->GetD3DDeviceContext(), &m_mainCamera);
	m_render->GetD3DDeviceContext()->RSSetState(0);
	m_sky->Draw(m_render->GetD3DDeviceContext(), &m_mainCamera);
	m_render->GetD3DDeviceContext()->RSSetState(0);
	m_render->Present();
}

void HardwareInstancing::Draw(ID3D11DeviceContext * context, const Camera * camera)
{
	context->IASetInputLayout(m_renderFX->GetInputLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride[] = { sizeof(VertexType),sizeof(InstanceData) };
	UINT offset[] = { 0, 0};

	ID3D11Buffer* vbs[] = { m_verticesBuffer.Get(),m_InstanceBuffer.Get() };

	auto renderFX = dynamic_cast<FXInstanceEffect*>(m_renderFX);

	renderFX->SetEyePosW(camera->GetPosition());

	renderFX->SetDirLights(mDirLights);

	context->IASetVertexBuffers(0, 2, vbs, stride, offset);
	context->IASetIndexBuffer(m_indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	XMMATRIX ViewProj = camera->ViewProj();
	
	renderFX->SetViewProjMatrix(ViewProj);

	renderFX->SetMaterial(mBoxMat);
	renderFX->SetDiffuseMap(mDiffuseMapSRV.Get());
	//renderFX->SetAlphaMap(mAlphaMapSRV.Get());
	renderFX->SetTexTransform(XMMatrixIdentity());
	auto activeTech = renderFX->Light2Tex;
	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		activeTech->GetPassByIndex(p)->Apply(0, context);
		context->DrawIndexedInstanced(m_Indices.size(), m_Instancies.size(), 0, 0, 0);
	}
}

void HardwareInstancing::BuildGeometryBuffer(ID3D11Device * device)
{
	Crate::BuildGeometryBuffer(device);

	const UINT xcount = 4;
	const UINT ycount = 4;

	m_Instancies.resize(xcount*ycount);

	float width = 20.0f;
	float height = 20.0f;
	float depth = 20.0f;
	float x = -0.5f*width;
	float y = -0.5f*height;
	float z = -0.5f*depth;
	float dx = width / (xcount - 1);
	float dy = height / (xcount - 1);
	float dz = depth / (xcount - 1);

	for (UINT i = 0;i<xcount;i++)
	{
		for (UINT j = 0;j<ycount;j++)
		{
			m_Instancies[i*xcount + j].world = XMFLOAT4X4(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				x + i*dx, y + j*dy, 0, 1.0f
			);
			m_Instancies[i*xcount + j].color = XMFLOAT4(MathHelper::RandF(0.f, 1.f), MathHelper::RandF(0.f, 1.f), MathHelper::RandF(0.f, 1.f), 1.f);
		}
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(InstanceData) * m_Instancies.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = m_Instancies.data();

	HR(device->CreateBuffer(&vbd, &vinitData, &m_InstanceBuffer));
}

void HardwareInstancing::UpdateScene(float deltaTime)
{
	m_sphere.SetPostion(0, 0, -20.f);
	m_sphere.SetDirLights(mDirLights);
	m_sphere.Update(deltaTime);
	m_sky->Update(deltaTime);
	Update(deltaTime);
	ApplicationD3D11::UpdateScene(deltaTime);
}

void HardwareInstancing::OnMouseButtonDown(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_RBUTTON) != 0)
	{
		XMVECTOR rayOrigin = XMVectorSet(0,0,0,1);
		XMVECTOR rayDir = XMVectorSet(0, 0, 0, 0);
		GetRayInViewSpace(x, y, m_mainCamera, &rayOrigin, &rayDir);
		DoIntersectTest(rayOrigin, rayDir, m_mainCamera);
	}
}

void HardwareInstancing::GetRayInViewSpace(int sx, int sy, const Camera & cam, XMVECTOR * origin, XMVECTOR * direction)
{
	assert(origin);
	assert(direction);

	XMFLOAT4X4 proj;
	XMStoreFloat4x4(&proj, cam.Proj());
	// Compute picking ray in view space.
	float vx = (+2.0f*sx / m_ClientWidth - 1.0f) /proj.m[0][0];
	float vy = (-2.0f*sy / m_ClientHeight + 1.0f) / proj.m[1][1];
	// Ray definition in view space.
	*origin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	*direction = XMVectorSet(vx, vy, 1.0f, 0.0f);
}

void HardwareInstancing::DoIntersectTest(FXMVECTOR origin, FXMVECTOR direction, const Camera& cam)
{
	auto view = cam.View();
	auto invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	float lastDistance = MathHelper::Infinity;
	int selectIndex = -1;
	for (UINT i = 0;i<m_Instancies.size();i++)
	{
		auto s = m_Instancies[i];
		auto world = XMLoadFloat4x4(&s.world);
		auto invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);
		auto tolocal = XMMatrixMultiply(invView, invWorld);
		XMVECTOR localOrigin = XMVector3TransformCoord(origin, tolocal);
		XMVECTOR localDir = XMVector3TransformNormal(direction, tolocal);
		localDir = XMVector3Normalize(localDir);
		BoundingBox box({ 0,0,0 }, { 2.5f,2.5f,2.5f });
		float distance = 0.f;
		
		if (box.Intersects(localOrigin, localDir,distance))
		{
			if (distance<lastDistance)
			{
				lastDistance = distance;
				selectIndex = i;
			}
		}
	}

	if (selectIndex>=0)
	{
		m_Instancies.erase(m_Instancies.begin() + selectIndex);
		D3D11_MAPPED_SUBRESOURCE mappedData;
		m_render->GetD3DDeviceContext()->Map(m_InstanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		InstanceData* dataView = reinterpret_cast<InstanceData*>(mappedData.pData);
		for (UINT i = 0;i<m_Instancies.size();i++)
		{
			dataView[i] = m_Instancies[i];
		}
		m_render->GetD3DDeviceContext()->Unmap(m_InstanceBuffer.Get(), 0);
	}
}
