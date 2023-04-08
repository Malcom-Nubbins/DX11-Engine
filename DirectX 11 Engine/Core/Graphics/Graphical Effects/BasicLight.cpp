#include "BasicLight.h"
#include "../../ApplicationNew.h"
#include "../../Loaders/ConfigLoader.h"
#include "../Core/Handlers/System Handlers/BufferClass.h"
#include "../Core/Handlers/System Handlers/ShaderClass.h"
#include "../Core/Handlers/System Handlers/RenderClass.h"

BasicLight::BasicLight()
	: m_TesselationHS(nullptr)
	, m_TesselationDS(nullptr)
	, m_TesselationVS(nullptr)
	, m_Viewport()
	, m_TesselationBufferPtr(nullptr)
	, m_CamLightBufferPtr(nullptr)
	, m_MatrixBufferPtr(nullptr)
	, m_ObjectValueBufferPtr(nullptr)
	, m_LightVS(nullptr)
	, m_LightPS(nullptr)
	, m_SimpleVertexInputLayout(nullptr)
	, m_RenderTargetTex2D(nullptr)
	, m_RenderTargetView(nullptr)
	, m_RenderTargetShaderResourceView(nullptr)
	, m_DepthStencilBuffer(nullptr)
	, m_DepthStencilView(nullptr)
	, m_FogValuesBufferPtr(nullptr)
	, m_bRenderWireframe(false)
	, m_MSAACount(0)
{
}

BasicLight::~BasicLight()
{
}

void BasicLight::Cleanup() const
{
	m_LightVS->Release();
	m_TesselationVS->Release();
	m_TesselationHS->Release();
	m_TesselationDS->Release();
	m_LightPS->Release();
	m_SimpleVertexInputLayout->Release();

	m_RenderTargetTex2D->Release();
	m_RenderTargetView->Release();
	m_RenderTargetShaderResourceView->Release();

	m_DepthStencilBuffer->Release();
	m_DepthStencilView->Release();

	m_FogValuesBufferPtr->Release();
	m_TesselationBufferPtr->Release();
	m_CamLightBufferPtr->Release();

	m_MatrixBufferPtr->Release();
	m_ObjectValueBufferPtr->Release();
}

void BasicLight::PreResize()
{
	if (m_RenderTargetTex2D != nullptr)
	{
		m_RenderTargetTex2D->Release();
		m_RenderTargetTex2D = nullptr;
	}

	if (m_RenderTargetView != nullptr)
	{
		m_RenderTargetView->Release();
		m_RenderTargetView = nullptr;
	}

	if (m_RenderTargetShaderResourceView != nullptr)
	{
		m_RenderTargetShaderResourceView->Release();
		m_RenderTargetShaderResourceView = nullptr;
	}

	if (m_DepthStencilBuffer != nullptr)
	{
		m_DepthStencilBuffer->Release();
		m_DepthStencilBuffer = nullptr;
	}

	if (m_DepthStencilView != nullptr)
	{
		m_DepthStencilView->Release();
		m_DepthStencilView = nullptr;
	}
}

void BasicLight::Resize(float newWidth, float newHeight)
{
	InitialiseRenderTargetAndDepthStencilViews(newWidth, newHeight);
	InitialiseViewport(newWidth, newHeight);
}

HRESULT BasicLight::Initialise(float windowWidth, float windowHeight)
{
	auto device = ApplicationNew::Get().GetDevice();

	int msaaCount = ApplicationNew::Get().GetConfigLoader()->GetSettingValue(SettingType::Graphics, "MSAA");
	if (msaaCount == -1)
	{
		msaaCount = 0;
	}

	m_MSAACount = static_cast<UINT>(msaaCount);

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

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MatrixBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &m_MatrixBufferPtr);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ObjectValuesBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &m_ObjectValueBufferPtr);

	InitialiseViewport(windowWidth, windowHeight);

	m_LightColourDayDiffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	m_LightColourDayAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_LightColourDaySpecular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);

	m_LightColourSunsetDiffuse = XMFLOAT4(0.89f, 0.59f, 0.27f, 1.0f);
	m_LightColourSunsetAmbient = XMFLOAT4(0.19f, 0.09f, 0.07f, 1.0f);
	m_LightColourSunsetSpecular = XMFLOAT4(0.89f, 0.59f, 0.27f, 1.0f);

	m_LightColourNightDiffuse = XMFLOAT4(0.01f, 0.02f, 0.04f, 1.0f);
	m_LightColourNightAmbient = XMFLOAT4(0.01f, 0.02f, 0.04f, 1.0f);
	m_LightColourNightSpecular = XMFLOAT4(0.01f, 0.02f, 0.04f, 1.0f);

	return S_OK;
}

void BasicLight::SetAsCurrentShader()
{
	ShaderClass::SetShadersAndInputLayout(m_LightVS, m_LightPS, m_SimpleVertexInputLayout);
}

void BasicLight::SetAsCurrentRenderTarget()
{
	RenderClass::SetRenderTargetAndDepthStencil(m_RenderTargetView, m_DepthStencilView);
}

void BasicLight::SetAsCurrentViewport()
{
	RenderClass::SetViewport(m_Viewport);
}

HRESULT BasicLight::InitialiseShaders()
{
	auto device = ApplicationNew::Get().GetDevice();

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HRESULT hr = ShaderClass::CreateVertexShader((WCHAR*)L"Core/Shaders/LightVS.cso", &m_LightVS, &m_SimpleVertexInputLayout, layout, ARRAYSIZE(layout));
	if (FAILED(hr))
		return hr;

	hr = ShaderClass::CreateVertexShader((WCHAR*)L"Core/Shaders/TesselationVS.cso", &m_TesselationVS, &m_SimpleVertexInputLayout, layout, ARRAYSIZE(layout));
	if (FAILED(hr))
	{
		return hr;
	}

	hr = ShaderClass::CreatePixelShader((WCHAR*)L"Core/Shaders/LightPS.cso", &m_LightPS);
	if (FAILED(hr))
		return hr;

	hr = ShaderClass::CreateHullShader((WCHAR*)L"Core/Shaders/TesselationHS.cso", &m_TesselationHS);
	if (FAILED(hr))
		return hr;

	hr = ShaderClass::CreateDomainShader((WCHAR*)L"Core/Shaders/TesselationDS.cso", &m_TesselationDS);
	if (FAILED(hr))
		return hr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(FogValuesBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &m_FogValuesBufferPtr);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(TesselationBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &m_TesselationBufferPtr);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CamLightBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &m_CamLightBufferPtr);

	return S_OK;
}

HRESULT BasicLight::InitialiseRenderTargetAndDepthStencilViews(float windowWidth, float windowHeight)
{
	auto device = ApplicationNew::Get().GetDevice();

	UINT sampleQuality;
	device->CheckMultisampleQualityLevels(DXGI_FORMAT_R32G32B32A32_FLOAT, m_MSAACount, &sampleQuality);

	D3D11_TEXTURE2D_DESC renderTargetTexDesc;
	renderTargetTexDesc.Width = static_cast<UINT>(windowWidth);
	renderTargetTexDesc.Height = static_cast<UINT>(windowHeight);
	renderTargetTexDesc.MipLevels = 1;
	renderTargetTexDesc.ArraySize = 1;
	renderTargetTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderTargetTexDesc.SampleDesc.Count = m_MSAACount;
	renderTargetTexDesc.SampleDesc.Quality = 0;
	renderTargetTexDesc.Usage = D3D11_USAGE_DEFAULT;
	renderTargetTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	renderTargetTexDesc.CPUAccessFlags = 0;
	renderTargetTexDesc.MiscFlags = 0;

	HRESULT hr = device->CreateTexture2D(&renderTargetTexDesc, nullptr, &m_RenderTargetTex2D);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create Render Texture (BasicLight)", L"Error", MB_OK);
		return hr;
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc{};
	renderTargetDesc.Format = renderTargetTexDesc.Format;
	renderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	renderTargetDesc.Texture2D.MipSlice = 0;

	hr = device->CreateRenderTargetView(m_RenderTargetTex2D, &renderTargetDesc, &m_RenderTargetView);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create Render Target View (BasicLight)", L"Error", MB_OK);
		return hr;
	}

#if defined(_DEBUG)
	char const rtvNamee[] = "BasicLightRTV";
	m_RenderTargetView->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(rtvNamee) - 1, rtvNamee);
#endif

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = renderTargetTexDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(m_RenderTargetTex2D, &srvDesc, &m_RenderTargetShaderResourceView);

	device->CheckMultisampleQualityLevels(DXGI_FORMAT_D24_UNORM_S8_UINT, m_MSAACount, &sampleQuality);

	D3D11_TEXTURE2D_DESC depthBufferDesc{};

	depthBufferDesc.Width = static_cast<UINT>(windowWidth);
	depthBufferDesc.Height = static_cast<UINT>(windowHeight);
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = m_MSAACount;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	hr = device->CreateTexture2D(&depthBufferDesc, nullptr, &m_DepthStencilBuffer);
	if (FAILED(hr))
		return hr;

	hr = device->CreateDepthStencilView(m_DepthStencilBuffer, nullptr, &m_DepthStencilView);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void BasicLight::InitialiseViewport(float windowWidth, float windowHeight)
{
	m_Viewport.Width = static_cast<FLOAT>(windowWidth);
	m_Viewport.Height = static_cast<FLOAT>(windowHeight);
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
}

void BasicLight::CalculateLightColour(DirectionalLight& sceneLight, float sunHeight, FogValuesBuffer& sceneFog)
{
	XMVECTOR diffuseDay = XMLoadFloat4(&m_LightColourDayDiffuse);
	XMVECTOR ambientDay = XMLoadFloat4(&m_LightColourDayAmbient);
	XMVECTOR specularDay = XMLoadFloat4(&m_LightColourDaySpecular);

	XMVECTOR diffuseSunset = XMLoadFloat4(&m_LightColourSunsetDiffuse);
	XMVECTOR ambientSunset = XMLoadFloat4(&m_LightColourSunsetAmbient);
	XMVECTOR specularSunset = XMLoadFloat4(&m_LightColourSunsetSpecular);

	XMVECTOR diffuseNight = XMLoadFloat4(&m_LightColourNightDiffuse);
	XMVECTOR ambientNight = XMLoadFloat4(&m_LightColourNightAmbient);
	XMVECTOR specularNight = XMLoadFloat4(&m_LightColourNightSpecular);

	if (sunHeight >= 20.0f)
	{
		XMStoreFloat4(&sceneLight.Diffuse, diffuseDay);
		XMStoreFloat4(&sceneLight.Ambient, ambientDay);
		XMStoreFloat4(&sceneLight.Specular, specularDay);
	}
	else if (sunHeight <= 20.0f && sunHeight > 0.0f)
	{
		XMVECTOR blendedLightDiffuse = XMVectorLerp(diffuseSunset, diffuseDay, sunHeight / 20.0f);
		XMVECTOR blendedLightAmbient = XMVectorLerp(ambientSunset, ambientDay, sunHeight / 20.0f);
		XMVECTOR blendedLightSpecular = XMVectorLerp(specularSunset, specularDay, sunHeight / 20.0f);
		
		XMStoreFloat4(&sceneLight.Diffuse, blendedLightDiffuse);
		XMStoreFloat4(&sceneLight.Ambient, blendedLightAmbient);
		XMStoreFloat4(&sceneLight.Specular, blendedLightSpecular);
	}
	else if(sunHeight <= 0.0f && sunHeight > -10.0f)
	{
		XMVECTOR blendedLightDiffuse = XMVectorLerp(diffuseSunset, diffuseNight, -sunHeight / 10.0f);
		XMVECTOR blendedLightAmbient = XMVectorLerp(ambientSunset, ambientNight, -sunHeight / 10.0f);
		XMVECTOR blendedLightSpecular = XMVectorLerp(specularSunset, specularNight, -sunHeight / 10.0f);
		
		XMStoreFloat4(&sceneLight.Diffuse, blendedLightDiffuse);
		XMStoreFloat4(&sceneLight.Ambient, blendedLightAmbient);
		XMStoreFloat4(&sceneLight.Specular, blendedLightSpecular);
	}
	else
	{
		XMStoreFloat4(&sceneLight.Diffuse, diffuseNight);
		XMStoreFloat4(&sceneLight.Ambient, ambientNight);
		XMStoreFloat4(&sceneLight.Specular, specularNight);
	}
}

void BasicLight::Render(Camera& camera, const DirectionalLight& sceneLight, const std::vector<PointLight>& pointLights, const SpotLight& spotLight, 
						const FogValuesBuffer& fogValues, const std::vector<SceneElement*>& sceneElements, Shadows& shadowClass)
{
	auto context = ApplicationNew::Get().GetContext();
	RenderClass::EnableZBuffer();

	BufferClass::SetVertexShaderBuffers(&m_MatrixBufferPtr, 0);

	BufferClass::SetPixelShaderBuffers(&m_ObjectValueBufferPtr, 0);
	BufferClass::SetPixelShaderBuffers(&m_FogValuesBufferPtr, 1);

	if (m_bRenderWireframe)
		RenderClass::SetRasterizerState(RasterizerType::WIREFRAME);
	else
		RenderClass::SetRasterizerState(RasterizerType::BACK_CULL);

	MatrixBuffer matBuffer;
	ObjectValuesBuffer objValBuffer;
	FogValuesBuffer fogValuesBuffer = fogValues;
	TesselationBuffer tessValues;
	CamLightBuffer camLightValues;

	const XMMATRIX view = XMLoadFloat4x4(&camera.GetView());
	const XMMATRIX proj = XMLoadFloat4x4(&camera.GetPerspectiveProj());
	const XMMATRIX shadowTransform = XMLoadFloat4x4(&shadowClass.GetShadowTransform());

	matBuffer.View = XMMatrixTranspose(view);
	matBuffer.Projection = XMMatrixTranspose(proj);
	matBuffer.ShadowTransform = shadowTransform;

	objValBuffer.DirLight = sceneLight;
	objValBuffer.SpotLight = spotLight;
	objValBuffer.EyePos = camera.GetPosition();

	tessValues.MaxTessDistance = 1.0f;
	tessValues.MinTessDistance = 50.0f;
	tessValues.MaxTessFactor = 10.0f;
	tessValues.MinTessFactor = 1.0f;

	camLightValues.EyePos = XMFLOAT4(camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z, 1.0f);
	camLightValues.LightVector = XMFLOAT4(sceneLight.LightDirection.x, sceneLight.LightDirection.y, sceneLight.LightDirection.z, 1.0f);

	if (!pointLights.empty())
		objValBuffer.UsePointLights = 0.0f;
	else
		objValBuffer.UsePointLights = 0.0f;

	objValBuffer.NumOfLights = static_cast<float>(pointLights.size());

	if (pointLights.size() < 96)
	{
		for (int i = 0; i < pointLights.size(); ++i)
		{
			objValBuffer.PointLight[i] = pointLights.at(i);
		}
	}
	else
	{
		for (int i = 0; i < 96; ++i)
		{
			objValBuffer.PointLight[i] = pointLights.at(i);
		}
	}

	context->PSSetSamplers(0, 1, ShaderClass::GetSamplerState(SamplerType::ANISOTROPIC));
	context->PSSetSamplers(1, 1, ShaderClass::GetSamplerState(SamplerType::SHADOW));

	SetAsCurrentRenderTarget();
	SetAsCurrentShader();
	SetAsCurrentViewport();

	ID3D11ShaderResourceView* shadowSRV = shadowClass.GetShadowSRV();
	context->PSSetShaderResources(1, 1, &shadowSRV);

	for (SceneElement* element : sceneElements)
	{
		Appearance* appearance = element->GetAppearance();
		Transform* transform = element->GetTransform();

		ObjectMaterial mat = appearance->GetObjectMaterial();
		objValBuffer.Surface.Ambient = mat.Ambient;
		objValBuffer.Surface.Diffuse = mat.Diffuse;
		objValBuffer.Surface.Specular = mat.Specular;

		matBuffer.World = XMMatrixTranspose(XMLoadFloat4x4(&transform->GetWorld()));
		ID3D11ShaderResourceView* tex;
		if (appearance->HasColourTexture())
		{
			tex = appearance->GetColourTex();
			context->PSSetShaderResources(0, 1, &tex);
			objValBuffer.UseColourTex = 1.0f;
		}
		else
		{
			objValBuffer.UseColourTex = 0.0f;
		}

		if (appearance->HasNormalMap())
		{
			objValBuffer.UseBumpMap = 1.0f;
			tex = appearance->GetNormalMap();
			context->PSSetShaderResources(2, 1, &tex);
		}
		else
		{
			objValBuffer.UseBumpMap = 0.0f;
		}

		if (appearance->HasDisplacementMap())
		{
			ShaderClass::SetShadersAndInputLayout(m_TesselationVS, m_LightPS, m_SimpleVertexInputLayout);

			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

			ID3D11ShaderResourceView* displacementMap = appearance->GetDisplacementMap();
			ShaderClass::SetHullAndDomainShaders(m_TesselationHS, m_TesselationDS);
			context->DSSetShaderResources(0, 1, &displacementMap);
			context->DSSetSamplers(0, 1, ShaderClass::GetSamplerState(SamplerType::LINEAR));

			BufferClass::SetVertexShaderBuffers(&m_TesselationBufferPtr, 1);
			BufferClass::SetVertexShaderBuffers(&m_CamLightBufferPtr, 2);

			BufferClass::SetDomainShaderBuffers(&m_MatrixBufferPtr, 0);
			BufferClass::SetDomainShaderBuffers(&m_TesselationBufferPtr, 1);
			BufferClass::SetDomainShaderBuffers(&m_CamLightBufferPtr, 2);
		}
		else
		{
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			ShaderClass::UnbindTesselationStages();
		}

		if (appearance->HasSpecularMap())
		{
			objValBuffer.UseSpecularMap = 1.0f;
			tex = appearance->GetSpecularMap();
			context->PSSetShaderResources(3, 1, &tex);
		}
		else
		{
			objValBuffer.UseSpecularMap = 0.0f;
		}

		if (element->IsAffectedByLight())
		{
			objValBuffer.AffectedByLight = 1.0f;
		}
		else
		{
			objValBuffer.AffectedByLight = 0.0f;
		}

		context->UpdateSubresource(m_MatrixBufferPtr, 0, nullptr, &matBuffer, 0, 0);
		context->UpdateSubresource(m_ObjectValueBufferPtr, 0, nullptr, &objValBuffer, 0, 0);
		context->UpdateSubresource(m_TesselationBufferPtr, 0, nullptr, &tessValues, 0, 0);
		context->UpdateSubresource(m_FogValuesBufferPtr, 0, nullptr, &fogValuesBuffer, 0, 0);
		context->UpdateSubresource(m_CamLightBufferPtr, 0, nullptr, &camLightValues, 0, 0);

		element->Draw();

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ShaderClass::UnbindTesselationStages();
		ShaderClass::SetShadersAndInputLayout(m_LightVS, m_LightPS, m_SimpleVertexInputLayout);
	}

	// Clear out the resource slots
	ID3D11ShaderResourceView* const srv[4] = { nullptr, nullptr, nullptr, nullptr };
	context->PSSetShaderResources(0, 4, srv);

	//terrain.Draw(matBuffer, objValBuffer, matrixBuffer, objectValueBuffer, nullptr);
}

void BasicLight::Render(Camera& camera, const DirectionalLight& sceneLight, const SpotLight& spotLight,
	const ::std::vector<SceneElement*>& sceneElements, Shadows& shadowClass)
{
	const auto emptyPointLightsVector = std::vector<PointLight>();
	auto fogVals = FogValuesBuffer();
	fogVals.UseFog = 0.0f;

	Render(camera, sceneLight, emptyPointLightsVector, spotLight, fogVals, sceneElements, shadowClass);
}
