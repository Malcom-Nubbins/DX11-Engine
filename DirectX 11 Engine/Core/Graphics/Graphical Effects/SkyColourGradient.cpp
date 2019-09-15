#include "SkyColourGradient.h"
#include "../../Loaders/ModelLoader.h"

SkyColourGradient::SkyColourGradient(const SystemHandlers& system)
	: _systemHandlers(system), _skyDomeElement(nullptr), _matrixBuffer(nullptr)
{
	_sceneCentre = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_colourGradientVS = nullptr;
	_colourGradientPS = nullptr;
	_inputLayout = nullptr;
	_gradientValuesBuffer = nullptr;
}

SkyColourGradient::~SkyColourGradient()
{
}

void SkyColourGradient::Cleanup() const
{
	_colourGradientVS->Release();
	_colourGradientPS->Release();
	_inputLayout->Release();
	_gradientValuesBuffer->Release();
}

HRESULT SkyColourGradient::Initialise()
{
	HRESULT hr;

	hr = InitialiseShaders();
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to Create Colour Gradient Shader", L"Error", MB_OK);
		return hr;
	}

	hr = InitialiseBuffers();
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to Create Colour Gradient Buffers", L"Error", MB_OK);
		return hr;
	}

	InitialiseSkydomeElement();

	return S_OK;
}

HRESULT SkyColourGradient::InitialiseShaders()
{
	HRESULT hr;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = _systemHandlers.GetShaderClass()->CreateVertexShader((WCHAR*)L"Core/Shaders/ColourGradientVS.hlsl", &_colourGradientVS , &_inputLayout, layout, ARRAYSIZE(layout));
	if (FAILED(hr))
		return hr;
	hr = _systemHandlers.GetShaderClass()->CreatePixelShader((WCHAR*)L"Core/Shaders/ColourGradientPS.hlsl", &_colourGradientPS);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT SkyColourGradient::InitialiseBuffers()
{
	HRESULT hr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(GradientValuesBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = _systemHandlers.GetD3DClass()->GetDevice()->CreateBuffer(&bd, nullptr, &_gradientValuesBuffer);
	if (FAILED(hr))
		return hr;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MatrixBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	_systemHandlers.GetD3DClass()->GetDevice()->CreateBuffer(&bd, nullptr, &_matrixBuffer);

	return S_OK;
}

void SkyColourGradient::InitialiseSkydomeElement()
{
	ObjectMaterial matte;
	matte.ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	matte.diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	matte.specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	auto* skyDomeTransform = new Transform();
	skyDomeTransform->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	skyDomeTransform->SetScale(XMFLOAT3(1.0f, 0.5f, 1.0f));
	skyDomeTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

	NewObjectMesh sphere;
	ModelLoader::LoadModel(_systemHandlers.GetD3DClass()->GetDevice(), L"Core/Resources/Objects/spherex.obj", sphere, false);

	auto appearance = new Appearance(sphere, matte);
	appearance->SetColourTexture(nullptr);

	_skyDomeElement = new SceneElement("Sky dome",  skyDomeTransform, appearance);
	_skyDomeElement->SetCastShadows(false);
	_skyDomeElement->SetAffectedByLight(false);
}

void SkyColourGradient::SetAsCurrentShader()
{
	_systemHandlers.GetShaderClass()->SetShadersAndInputLayout(_colourGradientVS, _colourGradientPS, _inputLayout);
}

void SkyColourGradient::Update(float deltaTime)
{
	_skyDomeElement->GetTransform()->SetPosition(_sceneCentre);
	_skyDomeElement->Update(deltaTime);
}

void SkyColourGradient::Render(const Camera& camera, const XMFLOAT3& sunPos)
{
	_systemHandlers.GetRenderClass()->DisableZBuffer();
	_systemHandlers.GetRenderClass()->SetRasterizerState(NO_CULL);

	_systemHandlers.GetBufferClass()->SetVertexShaderBuffers(&_matrixBuffer);
	_systemHandlers.GetBufferClass()->SetPixelShaderBuffers(&_gradientValuesBuffer);

	SetAsCurrentShader();

	GradientValuesBuffer colourValues;
	colourValues.horizonColourDay = XMFLOAT4(0.7f, 0.80f, 0.92f, 1.0f);
	colourValues.overheadColourDay = XMFLOAT4(0.17f, 0.26f, 0.69f, 1.0f);

	colourValues.horizonColourSunrise = XMFLOAT4(0.89f, 0.59f, 0.27f, 1.0f);
	colourValues.overheadColourSunrise = XMFLOAT4(0.29f, 0.38f, 0.58f, 1.0f);

	colourValues.horizonColourNight = XMFLOAT4(0.01f, 0.02f, 0.04f, 1.0f);
	colourValues.overheadColourNight = XMFLOAT4(0.01f, 0.02f, 0.04f, 1.0f);

	colourValues.sunPos = sunPos;

	MatrixBuffer matrixBufferValues;
	
	XMMATRIX view = XMLoadFloat4x4(&camera.GetView());
	XMMATRIX proj = XMLoadFloat4x4(&camera.GetPerspectiveProj());
	matrixBufferValues.View = XMMatrixTranspose(view);
	matrixBufferValues.Projection = XMMatrixTranspose(proj);

	XMMATRIX world = XMLoadFloat4x4(&_skyDomeElement->GetTransform()->GetWorld());
	matrixBufferValues.World = XMMatrixTranspose(world);

	_systemHandlers.GetD3DClass()->GetContext()->UpdateSubresource(_matrixBuffer, 0, nullptr, &matrixBufferValues, 0, 0);
	_systemHandlers.GetD3DClass()->GetContext()->UpdateSubresource(_gradientValuesBuffer, 0, nullptr, &colourValues, 0, 0);

	_skyDomeElement->Draw(_systemHandlers.GetD3DClass()->GetContext());
}