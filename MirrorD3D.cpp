#include "stdafx.h"
#include "MirrorD3D.h"
#include "DirectXTex.h"


MirrorD3D::MirrorD3D(const TCHAR * titleName, const TCHAR * className) :ApplicationD3D11(titleName, className)
{
	mDirLights[0].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	mDirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	mDirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	mRoomMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mRoomMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mRoomMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);

	// Reflected material is transparent so it blends into mirror.
	mMirrorMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mMirrorMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	mMirrorMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
}

MirrorD3D::~MirrorD3D()
{
}

bool MirrorD3D::InitApp(HINSTANCE hinstance)
{
	if (!ApplicationD3D11::InitApp(hinstance))
	{
		return false;
	}
	TexMetadata floor_texMeta;
	ScratchImage floor_diffuseImage;
	HR(LoadFromDDSFile(L"Textures/checkboard.dds", 0, &floor_texMeta, floor_diffuseImage));
	HR(CreateShaderResourceView(m_render->GetD3DDevice(), floor_diffuseImage.GetImages(), floor_diffuseImage.GetImageCount(), floor_texMeta, &m_floorDiffuseMapSRV));
	TexMetadata wall_texMeta;
	ScratchImage wall_diffuseImage;
	HR(LoadFromDDSFile(L"Textures/brick01.dds", 0, &wall_texMeta, wall_diffuseImage));
	HR(CreateShaderResourceView(m_render->GetD3DDevice(), wall_diffuseImage.GetImages(), wall_diffuseImage.GetImageCount(), wall_texMeta, &m_wallDiffuseMapSRV));
	this->Initialize(m_render->GetD3DDeviceContext());

	TexMetadata mirror_texMeta;
	ScratchImage mirror_diffuseImage;
	HR(LoadFromDDSFile(L"Textures/ice.dds", 0, &mirror_texMeta, mirror_diffuseImage));
	HR(CreateShaderResourceView(m_render->GetD3DDevice(), mirror_diffuseImage.GetImages(), mirror_diffuseImage.GetImageCount(), mirror_texMeta, &m_mirrorDiffuseMapSRV));

	m_crate.Initialize(m_render->GetD3DDeviceContext());
	m_reflectionCrate.Initialize(m_render->GetD3DDeviceContext());

	D3D11_BLEND_DESC noRenderTargetDBD;
	noRenderTargetDBD.AlphaToCoverageEnable = FALSE;
	noRenderTargetDBD.IndependentBlendEnable = FALSE;
	noRenderTargetDBD.RenderTarget[0].BlendEnable = FALSE;
	noRenderTargetDBD.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	noRenderTargetDBD.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	noRenderTargetDBD.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	noRenderTargetDBD.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	noRenderTargetDBD.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	noRenderTargetDBD.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	noRenderTargetDBD.RenderTarget[0].RenderTargetWriteMask = 0;

	HR(m_render->GetD3DDevice()->CreateBlendState(&noRenderTargetDBD, &m_noRenderTargetDBS));

	D3D11_DEPTH_STENCIL_DESC mirrorDesc;
	mirrorDesc.DepthEnable = TRUE;
	mirrorDesc.DepthFunc = D3D11_COMPARISON_LESS;
	mirrorDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	mirrorDesc.StencilEnable = TRUE;
	mirrorDesc.StencilReadMask = 0xff;
	mirrorDesc.StencilWriteMask = 0xff;

	mirrorDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	mirrorDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	mirrorDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	mirrorDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HR(m_render->GetD3DDevice()->CreateDepthStencilState(&mirrorDesc, &m_makeMirrorDSS));

	D3D11_DEPTH_STENCIL_DESC reflectionDesc;
	reflectionDesc.DepthEnable = TRUE;
	reflectionDesc.DepthFunc = D3D11_COMPARISON_LESS;
	reflectionDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	reflectionDesc.StencilEnable = TRUE;
	reflectionDesc.StencilReadMask = 0xff;
	reflectionDesc.StencilWriteMask = 0xff;

	reflectionDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	reflectionDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	reflectionDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	reflectionDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	reflectionDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	reflectionDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	reflectionDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	reflectionDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	HR(m_render->GetD3DDevice()->CreateDepthStencilState(&reflectionDesc, &m_reflectionDSS));

	D3D11_BLEND_DESC transparentDesc;
	transparentDesc.AlphaToCoverageEnable = FALSE;
	transparentDesc.IndependentBlendEnable = FALSE;
	transparentDesc.RenderTarget[0].BlendEnable = TRUE;
	transparentDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transparentDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(m_render->GetD3DDevice()->CreateBlendState(&transparentDesc, &m_transparentDBS));
	return true;
}

void MirrorD3D::BindFX(ID3D11Device * device)
{
	m_renderFX = FXEffectsFactory::GetFXBasicEffect(device);
}

void MirrorD3D::UpdateScene(float deltaTime)
{
	m_crate.SetScaling(0.3f, 0.3f, 0.3f);
	m_crate.SetPostion(0, 7.5, -25);
	m_reflectionCrate.SetScaling(0.3f, 0.3f, 0.3f);
	m_reflectionCrate.SetPostion(0, 7.5, 25);
	Update(deltaTime);
	m_crate.Update(deltaTime);
	m_reflectionCrate.Update(deltaTime);
	ApplicationD3D11::UpdateScene(deltaTime);
}

void MirrorD3D::RenderScene()
{
	m_render->Clear();

	//Render Room
	this->Draw(m_render->GetD3DDeviceContext(), &m_mainCamera);
	m_render->Present();
}

void MirrorD3D::BuildGeometryBuffer(ID3D11Device * device)
{
	// Create and specify geometry.  For this sample we draw a floor
	// and a wall with a mirror on it.  We put the floor, wall, and
	// mirror geometry in one vertex buffer.
	//
	//   |--------------|
	//   |              |
	//   |----|----|----|
	//   |Wall|Mirr|Wall|
	//   |    | or |    |
	//   /--------------/
	//  /   Floor      /
	// /--------------/

	m_Vertices.resize(30);

	// Floor: Observe we tile texture coordinates.
	m_Vertices[0] = VertexType({ -3.5f, 0.0f, -10.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 4.0f });
	m_Vertices[1] = VertexType({ -3.5f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, {0.0f, 0.0f });
	m_Vertices[2] = VertexType({7.5f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {4.0f, 0.0f});

	m_Vertices[3] = VertexType({-3.5f, 0.0f, -10.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 4.0f});
	m_Vertices[4] = VertexType({7.5f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {4.0f, 0.0f});
	m_Vertices[5] = VertexType({7.5f, 0.0f, -10.0f}, {0.0f, 1.0f, 0.0f}, {4.0f, 4.0f});

	// Wall: Observe we tile texture coordinates, and that we
	// leave a gap in the middle for the mirror.
	m_Vertices[6] = VertexType({-3.5f, 0.0f, 0.0f},{ 0.0f, 0.0f, -1.0f}, {0.0f, 2.0f});
	m_Vertices[7] = VertexType({-3.5f, 4.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f});
	m_Vertices[8] = VertexType({-2.5f, 4.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.5f, 0.0f});

	m_Vertices[9] = VertexType({-3.5f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 2.0f});
	m_Vertices[10] = VertexType({-2.5f, 4.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.5f, 0.0f});
	m_Vertices[11] = VertexType({-2.5f, 0.0f, 0.0f},{ 0.0f, 0.0f, -1.0f}, {0.5f, 2.0f});

	m_Vertices[12] = VertexType({2.5f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 2.0f});
	m_Vertices[13] = VertexType({2.5f, 4.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f});
	m_Vertices[14] = VertexType({7.5f, 4.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {2.0f, 0.0f});

	m_Vertices[15] = VertexType({2.5f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 2.0f});
	m_Vertices[16] = VertexType({7.5f, 4.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {2.0f, 0.0f});
	m_Vertices[17] = VertexType({7.5f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {2.0f, 2.0f});

	m_Vertices[18] = VertexType({-3.5f, 4.0f, 0.0f},{ 0.0f, 0.0f, -1.0f}, {0.0f, 1.0f});
	m_Vertices[19] = VertexType({-3.5f, 6.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f});
	m_Vertices[20] = VertexType({7.5f, 6.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {6.0f, 0.0f});

	m_Vertices[21] = VertexType({-3.5f, 4.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f});
	m_Vertices[22] = VertexType({7.5f, 6.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {6.0f, 0.0f});
	m_Vertices[23] = VertexType({7.5f, 4.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {6.0f, 1.0f});

	// Mirror
	m_Vertices[24] = VertexType({-2.5f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f});
	m_Vertices[25] = VertexType({-2.5f, 4.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f});
	m_Vertices[26] = VertexType({2.5f, 4.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f});

	m_Vertices[27] = VertexType({-2.5f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f});
	m_Vertices[28] = VertexType({2.5f, 4.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f});
	m_Vertices[29] = VertexType({2.5f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f});

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexType) * 30;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = m_Vertices.data();
	HR(m_render->GetD3DDevice()->CreateBuffer(&vbd, &vinitData, &m_verticesBuffer));
}

void MirrorD3D::Draw(ID3D11DeviceContext * context, const Camera * camera)
{
	context->IASetInputLayout(m_renderFX->GetInputLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	float blendFactor[4] = { 0.f,0.f,0.f,0.f };

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	XMMATRIX view = camera->View();
	XMMATRIX proj = camera->Proj();
	XMMATRIX viewProj = camera->ViewProj();

	auto renderFX = static_cast<FXBasicEffect*>(m_renderFX);

	renderFX->SetDirLights(mDirLights);
	renderFX->SetEyePosW(camera->GetPosition());
	renderFX->SetFogColor(Colors::Black);
	renderFX->SetFogStart(2.0f);
	renderFX->SetFogRange(45.f);

	ID3DX11EffectTechnique* activeTech = nullptr;
	D3DX11_TECHNIQUE_DESC techDesc;

	activeTech = renderFX->Light2TexFog;


	// draw wall and floor as normal
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0;p<techDesc.Passes;p++)
	{
		ID3DX11EffectPass* pass = activeTech->GetPassByIndex(p);

		context->IASetVertexBuffers(0, 1, m_verticesBuffer.GetAddressOf(), &stride, &offset);

		auto world = XMLoadFloat4x4(&m_World);
		auto worldInvTranspose = MathHelper::InverseTranspose(world);
		auto worldViewProj = world*viewProj;

		renderFX->SetWorld(world);
		renderFX->SetWorldInvTranspose(worldInvTranspose);
		renderFX->SetWorldViewProjMatrix(worldViewProj);
		renderFX->SetTexTransform(XMMatrixIdentity());
		renderFX->SetMaterial(mRoomMat);
		renderFX->SetDiffuseMap(m_floorDiffuseMapSRV.Get());

		pass->Apply(0, context);
		context->Draw(6, 0);

		renderFX->SetDiffuseMap(m_wallDiffuseMapSRV.Get());
		pass->Apply(0, context);
		context->Draw(18, 6);
	}

	m_crate.Draw(m_render->GetD3DDeviceContext(), &m_mainCamera);

	context->OMSetBlendState(m_noRenderTargetDBS.Get(), blendFactor, 0xffffffff);
	context->OMSetDepthStencilState(m_makeMirrorDSS.Get(), 1);

	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; p++)
	{
		ID3DX11EffectPass* pass = activeTech->GetPassByIndex(p);

		context->IASetVertexBuffers(0, 1, m_verticesBuffer.GetAddressOf(), &stride, &offset);

		auto world = XMLoadFloat4x4(&m_World);
		auto worldInvTranspose = MathHelper::InverseTranspose(world);
		auto worldViewProj = world*viewProj;

		renderFX->SetWorld(world);
		renderFX->SetWorldInvTranspose(worldInvTranspose);
		renderFX->SetWorldViewProjMatrix(worldViewProj);
		renderFX->SetTexTransform(XMMatrixIdentity());
		
		pass->Apply(0, context);
		context->Draw(6, 24);
	}

	context->OMSetBlendState(0, blendFactor, 0xffffffff);
	context->OMSetDepthStencilState(0, 0);

	context->OMSetDepthStencilState(m_reflectionDSS.Get(), 1);
	m_reflectionCrate.Draw(m_render->GetD3DDeviceContext(), &m_mainCamera);
	context->OMSetDepthStencilState(0, 0);

	context->OMSetBlendState(m_transparentDBS.Get(), blendFactor, 0xffffffff);

	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; p++)
	{
		ID3DX11EffectPass* pass = activeTech->GetPassByIndex(p);

		context->IASetVertexBuffers(0, 1, m_verticesBuffer.GetAddressOf(), &stride, &offset);

		auto world = XMLoadFloat4x4(&m_World);
		auto worldInvTranspose = MathHelper::InverseTranspose(world);
		auto worldViewProj = world*viewProj;

		renderFX->SetWorld(world);
		renderFX->SetWorldInvTranspose(worldInvTranspose);
		renderFX->SetWorldViewProjMatrix(worldViewProj);
		renderFX->SetTexTransform(XMMatrixIdentity());
		renderFX->SetMaterial(mMirrorMat);
		renderFX->SetDiffuseMap(m_mirrorDiffuseMapSRV.Get());

		pass->Apply(0, context);
		context->Draw(6, 24);
	}
	context->OMSetBlendState(0, blendFactor, 0xffffffff);
}

