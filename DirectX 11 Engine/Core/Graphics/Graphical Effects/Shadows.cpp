#include "Shadows.h"
#include "../../ApplicationNew.h"
#include "../Core/Handlers/System Handlers/BufferClass.h"
#include "../Core/Handlers/System Handlers/ShaderClass.h"
#include "../Core/Handlers/System Handlers/RenderClass.h"

Shadows::Shadows()
	: m_InputLayout(nullptr)
	, m_ShadowViewport()
	, m_ShadowDepthMatrixBufferPtr(nullptr)
{
	XMStoreFloat4x4(&m_LightProjectionMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_LightViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ShadowTransformMatrix, XMMatrixIdentity());

	m_ShadowDepthVS = nullptr;
	m_SceneBoundary.SphereCentre = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_SceneBoundary.SphereRadius = sqrtf(35.0f * 35.0f + 40.0f * 40.0f);
	m_LightDirection = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_LightPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

Shadows::~Shadows()
{
}

void Shadows::Cleanup()
{
	m_InputLayout->Release();
	m_ShadowDepthVS->Release();
	m_ShadowTex->Release();
	m_ShadowDepthStencilView->Release();
	m_ShadowSRV->Release();
	m_ShadowDepthMatrixBufferPtr->Release();
}

HRESULT Shadows::Initialise(float windowWidth, float windowHeight)
{
	HRESULT hr;
	hr = InitialiseShaders();
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to Create Shadow Depth Shader", L"Error", MB_OK);
		return hr;
	}

	hr = InitialiseDepthStencilView(windowWidth, windowHeight);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to Create Shadow Depth Stencil View", L"Error", MB_OK);
		return hr;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MatrixBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	ApplicationNew::Get().GetDevice()->CreateBuffer(&bd, nullptr, &m_ShadowDepthMatrixBufferPtr);

	InitialiseViewport(windowWidth, windowHeight);

	return S_OK;
}

void Shadows::PreResizeViews()
{
	m_ShadowTex->Release();
	m_ShadowSRV->Release();
	m_ShadowDepthStencilView->Release();
}

void Shadows::OnResize(float windowWidth, float windowHeight)
{
	InitialiseDepthStencilView(windowWidth, windowHeight);
}

HRESULT Shadows::InitialiseShaders()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HRESULT hr = ShaderClass::CreateVertexShader((WCHAR*)L"Core/Shaders/ShadowDepthVS.cso", &m_ShadowDepthVS, &m_InputLayout, layout, ARRAYSIZE(layout));
	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Shadows::InitialiseDepthStencilView(float windowWidth, float windowHeight)
{
	auto device = ApplicationNew::Get().GetDevice();

	HRESULT hr;
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = static_cast<UINT>(windowWidth);
	texDesc.Height = static_cast<UINT>(windowHeight);
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	hr = device->CreateTexture2D(&texDesc, 0, &m_ShadowTex);
	if (FAILED(hr))
		return hr;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	hr = device->CreateDepthStencilView(m_ShadowTex, &dsvDesc, &m_ShadowDepthStencilView);
	if (FAILED(hr))
		return hr;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = device->CreateShaderResourceView(m_ShadowTex, &srvDesc, &m_ShadowSRV);
	if (FAILED(hr))
		return hr;
	return S_OK;
}

void Shadows::InitialiseViewport(float windowWidth, float windowHeight)
{
	m_ShadowViewport.Width = static_cast<FLOAT>(windowWidth);
	m_ShadowViewport.Height = static_cast<FLOAT>(windowHeight);
	m_ShadowViewport.MinDepth = 0.0f;
	m_ShadowViewport.MaxDepth = 1.0f;
	m_ShadowViewport.TopLeftX = 0;
	m_ShadowViewport.TopLeftY = 0;
}

void Shadows::BuildShadowTransform()
{
	const XMVECTOR lightDirVec = XMLoadFloat3(&m_LightDirection);
	const XMVECTOR lightPos = 2.0f * m_SceneBoundary.SphereRadius * lightDirVec;
	XMStoreFloat3(&m_LightPosition, lightPos);

	const XMVECTOR targetPos = XMLoadFloat3(&m_SceneBoundary.SphereCentre);

	const XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	const XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

	const float l = sphereCenterLS.x - m_SceneBoundary.SphereRadius;
	const float b = sphereCenterLS.y - m_SceneBoundary.SphereRadius;
	const float n = sphereCenterLS.z - m_SceneBoundary.SphereRadius;
	const float r = sphereCenterLS.x + m_SceneBoundary.SphereRadius;
	const float t = sphereCenterLS.y + m_SceneBoundary.SphereRadius;
	const float f = sphereCenterLS.z + m_SceneBoundary.SphereRadius;

	const XMMATRIX P = XMMatrixPerspectiveOffCenterLH(l, r, b, t, n, f);

	const XMMATRIX textureSpace(0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	const XMMATRIX ViewProj = XMMatrixMultiply(V, P);

	const XMMATRIX S = XMMatrixTranspose(XMMatrixMultiply(ViewProj, textureSpace));

	XMStoreFloat4x4(&m_LightViewMatrix, V);
	XMStoreFloat4x4(&m_LightProjectionMatrix, P);
	XMStoreFloat4x4(&m_ShadowTransformMatrix, S);
}

void Shadows::SetSceneCentre(XMFLOAT3 newCentre)
{
	m_SceneBoundary.SphereCentre.x = newCentre.x;
	m_SceneBoundary.SphereCentre.z = newCentre.z;
}

void Shadows::UpdateLightDirection(XMFLOAT3 lightDirection)
{
	m_LightDirection = lightDirection;
}

void Shadows::Render(const std::vector<SceneElement*>& sceneElements)
{
	auto context = ApplicationNew::Get().GetContext();

	RenderClass::SetRasterizerState(RasterizerType::SHADOWDEPTH);
	SetAsCurrentViewport();
	SetAsCurrentDepthStencil();
	SetAsCurrentShader();
	context->PSSetShader(nullptr, nullptr, 0);
	BufferClass::SetVertexShaderBuffers(&m_ShadowDepthMatrixBufferPtr);

	ShadowDepthMatrixBuffer shadowDepthMatrixBuffer;

	XMMATRIX view = XMLoadFloat4x4(&m_LightViewMatrix);
	XMMATRIX proj = XMLoadFloat4x4(&m_LightProjectionMatrix);

	shadowDepthMatrixBuffer.LightView = XMMatrixTranspose(view);
	shadowDepthMatrixBuffer.LightProjection = XMMatrixTranspose(proj);

	for (SceneElement* element : sceneElements)
	{
		if (element->CanCastShadows())
		{
			shadowDepthMatrixBuffer.World = XMMatrixTranspose(XMLoadFloat4x4(&element->GetTransform()->GetWorld()));
			context->UpdateSubresource(m_ShadowDepthMatrixBufferPtr, 0, nullptr, &shadowDepthMatrixBuffer, 0, 0);
			element->Draw();
		}
	}

	//terrain.ShadowDraw(shadowDepthMatrixBuffer, m_ShadowDepthMatrixBufferPtr);
}

void Shadows::SetAsCurrentShader()
{
	ShaderClass::SetShadersAndInputLayout(m_ShadowDepthVS, nullptr, m_InputLayout);
}

void Shadows::SetAsCurrentDepthStencil()
{
	RenderClass::SetRenderTargetAndDepthStencil(nullptr, m_ShadowDepthStencilView);
}

void Shadows::SetAsCurrentViewport()
{
	RenderClass::SetViewport(m_ShadowViewport);
}
