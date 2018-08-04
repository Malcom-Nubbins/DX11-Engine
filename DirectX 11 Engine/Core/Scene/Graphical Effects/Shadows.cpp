#include "Shadows.h"

Shadows::Shadows(D3DClass * d3dClass, RenderClass * renderClass, ShaderClass * shaderClass, BufferClass * bufferClass) 
	: _d3dClass(d3dClass), _renderClass(renderClass), _shaderClass(shaderClass), _bufferClass(bufferClass)
{
	_shadowDepthVS = nullptr;
	_sceneBoundary.sphereCentre = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_sceneBoundary.sphereRadius = sqrtf(35.0f * 35.0f + 40.0f * 40.0f);
	_lightDirection = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_lightPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

Shadows::~Shadows()
{
}

void Shadows::Cleanup()
{
	_shadowDepthVS->Release();
	_ShadowTex->Release();
	_ShadowDepthStencilView->Release();
	_ShadowSRV->Release();
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
	_d3dClass->GetDevice()->CreateBuffer(&bd, nullptr, &_shadowDepthMatrixBuffer);

	InitialiseViewport(windowWidth, windowHeight);

	return S_OK;
}

HRESULT Shadows::InitialiseShaders()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = _shaderClass->CreateVertexShader((WCHAR*)L"Core/Shaders/ShadowDepthVS.hlsl", &_shadowDepthVS, &_inputLayout, layout, ARRAYSIZE(layout));
	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Shadows::InitialiseDepthStencilView(float windowWidth, float windowHeight)
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = windowWidth;
	texDesc.Height = windowHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	hr = _d3dClass->GetDevice()->CreateTexture2D(&texDesc, 0, &_ShadowTex);
	if (FAILED(hr))
		return hr;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	hr = _d3dClass->GetDevice()->CreateDepthStencilView(_ShadowTex, &dsvDesc, &_ShadowDepthStencilView);
	if (FAILED(hr))
		return hr;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = _d3dClass->GetDevice()->CreateShaderResourceView(_ShadowTex, &srvDesc, &_ShadowSRV);
	if (FAILED(hr))
		return hr;
	return S_OK;
}

void Shadows::InitialiseViewport(float windowWidth, float windowHeight)
{
	_shadowViewport.Width = (FLOAT)windowWidth;
	_shadowViewport.Height = (FLOAT)windowHeight;
	_shadowViewport.MinDepth = 0.0f;
	_shadowViewport.MaxDepth = 1.0f;
	_shadowViewport.TopLeftX = 0;
	_shadowViewport.TopLeftY = 0;
}

void Shadows::BuildShadowTransform()
{
	XMVECTOR lightDirVec = XMLoadFloat3(&_lightDirection);
	XMVECTOR lightPos = 2.0f * _sceneBoundary.sphereRadius * lightDirVec;
	XMStoreFloat3(&_lightPosition, lightPos);

	XMVECTOR targetPos = XMLoadFloat3(&_sceneBoundary.sphereCentre);

	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

	float l = sphereCenterLS.x - _sceneBoundary.sphereRadius;
	float b = sphereCenterLS.y - _sceneBoundary.sphereRadius;
	float n = sphereCenterLS.z - _sceneBoundary.sphereRadius;
	float r = sphereCenterLS.x + _sceneBoundary.sphereRadius;
	float t = sphereCenterLS.y + _sceneBoundary.sphereRadius;
	float f = sphereCenterLS.z + _sceneBoundary.sphereRadius;

	XMMATRIX P = XMMatrixPerspectiveOffCenterLH(l, r, b, t, n, f);

	const XMMATRIX textureSpace(0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX ViewProj = XMMatrixMultiply(V, P);

	XMMATRIX S = XMMatrixTranspose(XMMatrixMultiply(ViewProj, textureSpace));

	XMStoreFloat4x4(&_lightViewMatrix, V);
	XMStoreFloat4x4(&_lightProjectionMatrix, P);
	XMStoreFloat4x4(&_shadowTransformMatrix, S);
}

void Shadows::SetSceneCentre(XMFLOAT3 newCentre)
{
	_sceneBoundary.sphereCentre.x = newCentre.x;
	_sceneBoundary.sphereCentre.z = newCentre.z;
}

void Shadows::UpdateLightDirection(XMFLOAT3 lightDirection)
{
	_lightDirection = lightDirection;
}

void Shadows::Render(const std::vector<SceneElement*>& sceneElements, DiamondSquareTerrain& terrain)
{
	_renderClass->SetRasterizerState(SHADOWDEPTH);
	SetAsCurrentViewport();
	SetAsCurrentDepthStencil();
	SetAsCurrentShader();
	_bufferClass->SetVertexShaderBuffers(&_shadowDepthMatrixBuffer);

	ShadowDepthMatrixBuffer shadowDepthMatrixBuffer;

	XMMATRIX view = XMLoadFloat4x4(&_lightViewMatrix);
	XMMATRIX proj = XMLoadFloat4x4(&_lightProjectionMatrix);

	shadowDepthMatrixBuffer.LightView = XMMatrixTranspose(view);
	shadowDepthMatrixBuffer.LightProjection = XMMatrixTranspose(proj);

	for (SceneElement* element : sceneElements)
	{
		if (element->CanCastShadows())
		{
			shadowDepthMatrixBuffer.World = XMMatrixTranspose(XMLoadFloat4x4(&element->GetWorld()));
			_d3dClass->GetContext()->UpdateSubresource(_shadowDepthMatrixBuffer, 0, nullptr, &shadowDepthMatrixBuffer, 0, 0);
			element->Draw(_d3dClass->GetContext());
		}
	}

	//terrain.ShadowDraw(shadowDepthMatrixBuffer, _shadowDepthMatrixBuffer);
}

void Shadows::SetAsCurrentShader()
{
	_shaderClass->SetShadersAndInputLayout(_shadowDepthVS, nullptr, _inputLayout);
}

void Shadows::SetAsCurrentDepthStencil()
{
	_renderClass->SetRenderTargetAndDepthStencil(nullptr, _ShadowDepthStencilView);
}

void Shadows::SetAsCurrentViewport()
{
	_renderClass->SetViewport(_shadowViewport);
}
