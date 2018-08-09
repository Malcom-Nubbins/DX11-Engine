#include "BasicLight.h"

BasicLight::BasicLight(D3DClass* d3dClass, ShaderClass* shaderClass, RenderClass* renderClass, BufferClass* bufferClass)
	: _d3dClass(d3dClass), _shaderClass(shaderClass), _renderClass(renderClass), _bufferClass(bufferClass)
{
	_lightVS = nullptr;
	_lightPS = nullptr;
	_simpleVertexInputLayout = nullptr;

	_renderTargetTex2D = nullptr;
	_renderTargetView = nullptr;
	_renderTargetSRV = nullptr;

	_depthStencilBuffer = nullptr;
	_depthStencilView = nullptr;

	_fogValuesBuffer = nullptr;

	_renderWireframe = false;
}

BasicLight::~BasicLight()
{
}

void BasicLight::Cleanup()
{
	_lightVS->Release();
	_lightPS->Release();
	_simpleVertexInputLayout->Release();

	_renderTargetTex2D->Release();
	_renderTargetView->Release();
	_renderTargetSRV->Release();

	_depthStencilBuffer->Release();
	_depthStencilView->Release();

	_fogValuesBuffer->Release();
}

void BasicLight::Resize(float newWidth, float newHeight)
{
	_renderTargetView->Release();
	_depthStencilBuffer->Release();
	_depthStencilView->Release();

	InitialiseRenderTargetAndDepthStencilViews(newWidth, newHeight);
	InitialiseViewport(newWidth, newHeight);
}

HRESULT BasicLight::Initialise(float windowWidth, float windowHeight)
{
	HRESULT hr;
	hr = InitialiseShaders();
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to Create Basic Light Shader", L"Error", MB_OK);
		return hr;
	}

	hr = InitialiseRenderTargetAndDepthStencilViews(windowWidth, windowHeight);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to Create Basic Light RTV & DSV", L"Error", MB_OK);
		return hr;
	}

	InitialiseViewport(windowWidth, windowHeight);

	_lightColourDayDiffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	_lightColourDayAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	_lightColourDaySpecular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);

	_lightColourSunsetDiffuse = XMFLOAT4(0.89f, 0.59f, 0.27f, 1.0f);
	_lightColourSunsetAmbient = XMFLOAT4(0.19f, 0.09f, 0.07f, 1.0f);
	_lightColourSunsetSpecular = XMFLOAT4(0.89f, 0.59f, 0.27f, 1.0f);

	_lightColourNightDiffuse = XMFLOAT4(0.01f, 0.02f, 0.04f, 1.0f);
	_lightColourNightAmbient = XMFLOAT4(0.01f, 0.02f, 0.04f, 1.0f);
	_lightColourNightSpecular = XMFLOAT4(0.01f, 0.02f, 0.04f, 1.0f);

	return S_OK;
}

void BasicLight::SetAsCurrentShader()
{
	_shaderClass->SetShadersAndInputLayout(_lightVS, _lightPS, _simpleVertexInputLayout);
}

void BasicLight::SetAsCurrentRenderTarget()
{
	_renderClass->SetRenderTargetAndDepthStencil(_renderTargetView, _depthStencilView);
}

void BasicLight::SetAsCurrentViewport()
{
	_renderClass->SetViewport(_viewport);
}

HRESULT BasicLight::InitialiseShaders()
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

	hr = _shaderClass->CreateVertexShader((WCHAR*)L"Core/Shaders/LightVS.hlsl", &_lightVS, &_simpleVertexInputLayout, layout, ARRAYSIZE(layout));
	if (FAILED(hr))
		return hr;
	hr = _shaderClass->CreatePixelShader((WCHAR*)L"Core/Shaders/LightPS.hlsl", &_lightPS);
	if (FAILED(hr))
		return hr;

	hr = _shaderClass->CreateHullShader((WCHAR*)L"Core/Shaders/TesselationHS.hlsl", &_tesselationHS);

	hr = _shaderClass->CreateDomainShader((WCHAR*)L"Core/Shaders/TesselationDS.hlsl", &_tesselationDS);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(FogValuesBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	_d3dClass->GetDevice()->CreateBuffer(&bd, nullptr, &_fogValuesBuffer);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(TesselationBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	_d3dClass->GetDevice()->CreateBuffer(&bd, nullptr, &_tesselationBuffer);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CamLightBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	_d3dClass->GetDevice()->CreateBuffer(&bd, nullptr, &_camLightBuffer);

	return S_OK;
}

HRESULT BasicLight::InitialiseRenderTargetAndDepthStencilViews(float windowWidth, float windowHeight)
{
	HRESULT hr;
	
	D3D11_TEXTURE2D_DESC renderTargetTexDesc;
	renderTargetTexDesc.Width = static_cast<UINT>(windowWidth);
	renderTargetTexDesc.Height = static_cast<UINT>(windowHeight);
	renderTargetTexDesc.MipLevels = 1;
	renderTargetTexDesc.ArraySize = 1;
	renderTargetTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderTargetTexDesc.SampleDesc.Count = 1;
	renderTargetTexDesc.SampleDesc.Quality = 0;
	renderTargetTexDesc.Usage = D3D11_USAGE_DEFAULT;
	renderTargetTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	renderTargetTexDesc.CPUAccessFlags = 0;
	renderTargetTexDesc.MiscFlags = 0;

	_d3dClass->GetDevice()->CreateTexture2D(&renderTargetTexDesc, nullptr, &_renderTargetTex2D);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc;
	renderTargetDesc.Format = renderTargetTexDesc.Format;
	renderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetDesc.Texture2D.MipSlice = 0;

	hr = _d3dClass->GetDevice()->CreateRenderTargetView(_renderTargetTex2D, &renderTargetDesc, &_renderTargetView);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create Render Target View (BasicLight)", L"Error", MB_OK);
		return hr;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = renderTargetTexDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	_d3dClass->GetDevice()->CreateShaderResourceView(_renderTargetTex2D, &srvDesc, &_renderTargetSRV);

	UINT sampleCount = 1;
	D3D11_TEXTURE2D_DESC depthBufferDesc;

	depthBufferDesc.Width = windowWidth;
	depthBufferDesc.Height = windowHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = sampleCount;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	hr = _d3dClass->GetDevice()->CreateTexture2D(&depthBufferDesc, nullptr, &_depthStencilBuffer);
	if (FAILED(hr))
		return hr;

	hr = _d3dClass->GetDevice()->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void BasicLight::InitialiseViewport(float windowWidth, float windowHeight)
{
	_viewport.Width = (FLOAT)windowWidth;
	_viewport.Height = (FLOAT)windowHeight;
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;
	_viewport.TopLeftX = 0;
	_viewport.TopLeftY = 0;
}

void BasicLight::CalculateLightColour(DirectionalLight& sceneLight, float sunHeight, FogValuesBuffer& sceneFog)
{
	XMVECTOR diffuseDay = XMLoadFloat4(&_lightColourDayDiffuse);
	XMVECTOR ambientDay = XMLoadFloat4(&_lightColourDayAmbient);
	XMVECTOR specularDay = XMLoadFloat4(&_lightColourDaySpecular);

	XMVECTOR diffuseSunset = XMLoadFloat4(&_lightColourSunsetDiffuse);
	XMVECTOR ambientSunset = XMLoadFloat4(&_lightColourSunsetAmbient);
	XMVECTOR specularSunset = XMLoadFloat4(&_lightColourSunsetSpecular);

	XMVECTOR diffuseNight = XMLoadFloat4(&_lightColourNightDiffuse);
	XMVECTOR ambientNight = XMLoadFloat4(&_lightColourNightAmbient);
	XMVECTOR specularNight = XMLoadFloat4(&_lightColourNightSpecular);

	if (sunHeight >= 20.0f)
	{
		XMStoreFloat4(&sceneLight.diffuse, diffuseDay);
		XMStoreFloat4(&sceneLight.ambient, ambientDay);
		XMStoreFloat4(&sceneLight.specular, specularDay);
	}
	else if (sunHeight <= 20.0f && sunHeight > 0.0f)
	{
		XMVECTOR blendedLightDiffuse = XMVectorLerp(diffuseSunset, diffuseDay, sunHeight / 20.0f);
		XMVECTOR blendedLightAmbient = XMVectorLerp(ambientSunset, ambientDay, sunHeight / 20.0f);
		XMVECTOR blendedLightSpecular = XMVectorLerp(specularSunset, specularDay, sunHeight / 20.0f);
		
		XMStoreFloat4(&sceneLight.diffuse, blendedLightDiffuse);
		XMStoreFloat4(&sceneLight.ambient, blendedLightAmbient);
		XMStoreFloat4(&sceneLight.specular, blendedLightSpecular);
	}
	else if(sunHeight <= 0.0f && sunHeight > -10.0f)
	{
		XMVECTOR blendedLightDiffuse = XMVectorLerp(diffuseSunset, diffuseNight, -sunHeight / 10.0f);
		XMVECTOR blendedLightAmbient = XMVectorLerp(ambientSunset, ambientNight, -sunHeight / 10.0f);
		XMVECTOR blendedLightSpecular = XMVectorLerp(specularSunset, specularNight, -sunHeight / 10.0f);
		
		XMStoreFloat4(&sceneLight.diffuse, blendedLightDiffuse);
		XMStoreFloat4(&sceneLight.ambient, blendedLightAmbient);
		XMStoreFloat4(&sceneLight.specular, blendedLightSpecular);
	}
	else
	{
		XMStoreFloat4(&sceneLight.diffuse, diffuseNight);
		XMStoreFloat4(&sceneLight.ambient, ambientNight);
		XMStoreFloat4(&sceneLight.specular, specularNight);
	}
}

void BasicLight::Render(const Camera& camera, const DirectionalLight& sceneLight, const std::vector<PointLight>& pointLights, const SpotLight& spotLight, 
						const FogValuesBuffer& fogValues, const std::vector<SceneElement*>& sceneElements, DiamondSquareTerrain& terrain, 
						ID3D11Buffer* matrixBuffer, ID3D11Buffer* objectValueBuffer, Shadows& shadowClass)
{
	_renderClass->EnableZBuffer();

	_bufferClass->SetVertexShaderBuffers(&matrixBuffer, 0);

	_bufferClass->SetPixelShaderBuffers(&objectValueBuffer, 0);
	_bufferClass->SetPixelShaderBuffers(&_fogValuesBuffer, 1);

	if (_renderWireframe)
		_renderClass->SetRasterizerState(WIREFRAME);
	else
		_renderClass->SetRasterizerState(BACK_CULL);

	MatrixBuffer matBuffer;
	ObjectValuesBuffer objValBuffer;
	FogValuesBuffer fogValuesBuffer = fogValues;
	TesselationBuffer tessValues;
	CamLightBuffer camLightValues;

	XMMATRIX view = XMLoadFloat4x4(&camera.GetView());
	XMMATRIX proj = XMLoadFloat4x4(&camera.GetPerspectiveProj());
	XMMATRIX shadowTransform = XMLoadFloat4x4(&shadowClass.GetShadowTransform());

	matBuffer.View = XMMatrixTranspose(view);
	matBuffer.Projection = XMMatrixTranspose(proj);
	matBuffer.ShadowTransform = shadowTransform;

	objValBuffer.dirLight = sceneLight;
	objValBuffer.spotLight = spotLight;
	objValBuffer.EyePos = camera.GetPosition();

	tessValues.MaxTessDistance = 1.0f;
	tessValues.MinTessDistance = 50.0f;
	tessValues.MaxTessFactor = 10.0f;
	tessValues.MinTessFactor = 1.0f;

	camLightValues.EyePos = XMFLOAT4(camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z, 1.0f);
	camLightValues.LightVector = XMFLOAT4(sceneLight.lightDirection.x, sceneLight.lightDirection.y, sceneLight.lightDirection.z, 1.0f);

	if (pointLights.size() > 0)
		objValBuffer.usePointLights = 0.0f;
	else
		objValBuffer.usePointLights = 0.0f;

	objValBuffer.numOfLights = pointLights.size();

	if (pointLights.size() < 96)
	{
		for (int i = 0; i < pointLights.size(); ++i)
		{
			objValBuffer.pointLight[i] = pointLights.at(i);
		}
	}
	else
	{
		for (int i = 0; i < 96; ++i)
		{
			objValBuffer.pointLight[i] = pointLights.at(i);
		}
	}

	_d3dClass->GetContext()->PSSetSamplers(0, 1, _shaderClass->GetSamplerState(ANISOTROPIC));
	_d3dClass->GetContext()->PSSetSamplers(1, 1, _shaderClass->GetSamplerState(SHADOW));

	SetAsCurrentRenderTarget();
	SetAsCurrentShader();
	SetAsCurrentViewport();

	ID3D11ShaderResourceView* shadowSRV = shadowClass.GetShadowSRV();
	_d3dClass->GetContext()->PSSetShaderResources(1, 1, &shadowSRV);

	for (SceneElement* element : sceneElements)
	{
		Appearance* appearance = element->GetAppearance();
		Transform* transform = element->GetTransform();

		ObjectMaterial mat = appearance->GetObjectMaterial();
		objValBuffer.surface.ambient = mat.ambient;
		objValBuffer.surface.diffuse = mat.diffuse;
		objValBuffer.surface.specular = mat.specular;

		matBuffer.World = XMMatrixTranspose(XMLoadFloat4x4(&transform->GetWorld()));
		ID3D11ShaderResourceView* tex;
		if (appearance->HasColourTexture())
		{
			tex = appearance->GetColourTex();
			_d3dClass->GetContext()->PSSetShaderResources(0, 1, &tex);
			objValBuffer.useColourTex = 1.0f;
		}
		else
		{
			objValBuffer.useColourTex = 0.0f;
		}

		if (appearance->HasNormalMap())
		{
			objValBuffer.useBumpMap = 1.0f;
			tex = appearance->GetNormalMap();
			_d3dClass->GetContext()->PSSetShaderResources(2, 1, &tex);
		}
		else
		{
			objValBuffer.useBumpMap = 0.0f;
		}

		if (appearance->HasDisplacementMap())
		{
			_d3dClass->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

			ID3D11ShaderResourceView* displacementMap = appearance->GetDisplacementMap();
			_shaderClass->SetHullAndDomainShaders(_tesselationHS, _tesselationDS);
			_d3dClass->GetContext()->DSSetShaderResources(0, 1, &displacementMap);
			_d3dClass->GetContext()->DSSetSamplers(0, 1, _shaderClass->GetSamplerState(LINEAR));

			_bufferClass->SetVertexShaderBuffers(&_tesselationBuffer, 1);
			_bufferClass->SetVertexShaderBuffers(&_camLightBuffer, 2);

			_bufferClass->SetDomainShaderBuffers(&matrixBuffer, 0);
			_bufferClass->SetDomainShaderBuffers(&_tesselationBuffer, 1);
			_bufferClass->SetDomainShaderBuffers(&_camLightBuffer, 2);
		}
		else
		{
			_d3dClass->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			_shaderClass->UnbindTesselationStages();
		}

		if (appearance->HasSpecularMap())
		{
			objValBuffer.useSpecularMap = 1.0f;
			tex = appearance->GetSpecularMap();
			_d3dClass->GetContext()->PSSetShaderResources(3, 1, &tex);
		}
		else
		{
			objValBuffer.useSpecularMap = 0.0f;
		}

		if (element->IsAffectedByLight())
		{
			objValBuffer.affectedByLight = 1.0f;
		}
		else
		{
			objValBuffer.affectedByLight = 0.0f;
		}

		_d3dClass->GetContext()->UpdateSubresource(matrixBuffer, 0, nullptr, &matBuffer, 0, 0);
		_d3dClass->GetContext()->UpdateSubresource(objectValueBuffer, 0, nullptr, &objValBuffer, 0, 0);
		_d3dClass->GetContext()->UpdateSubresource(_tesselationBuffer, 0, nullptr, &tessValues, 0, 0);
		_d3dClass->GetContext()->UpdateSubresource(_fogValuesBuffer, 0, nullptr, &fogValuesBuffer, 0, 0);
		_d3dClass->GetContext()->UpdateSubresource(_camLightBuffer, 0, nullptr, &camLightValues, 0, 0);

		element->Draw(_d3dClass->GetContext());
	}

	//terrain.Draw(matBuffer, objValBuffer, matrixBuffer, objectValueBuffer, nullptr);
}
