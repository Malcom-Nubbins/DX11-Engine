#include "SkyColourGradient.h"
#include "../../Loaders/ModelLoader.h"
#include "../../ApplicationNew.h"
#include "../Core/Handlers/System Handlers/BufferClass.h"
#include "../Core/Handlers/System Handlers/ShaderClass.h"
#include "../Core/Handlers/System Handlers/RenderClass.h"

SkyColourGradient::SkyColourGradient()
	: m_SkyDomeElement(nullptr), m_MatrixBufferPtr(nullptr), m_SkySRV(nullptr)
{
	m_SceneCentre = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_ColourGradientVS = nullptr;
	m_ColourGradientPS = nullptr;
	m_InputLayout = nullptr;
	m_GradientValuesBufferPtr = nullptr;
}

SkyColourGradient::~SkyColourGradient()
{
}

void SkyColourGradient::Cleanup() const
{
	m_SkyDomeElement->Cleanup();
	m_ColourGradientVS->Release();
	m_ColourGradientPS->Release();
	m_InputLayout->Release();
	m_GradientValuesBufferPtr->Release();
	m_MatrixBufferPtr->Release();
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

	hr = ShaderClass::CreateVertexShader((WCHAR*)L"Core/Shaders/ColourGradientVS.cso", &m_ColourGradientVS, &m_InputLayout, layout, ARRAYSIZE(layout));
	if (FAILED(hr))
		return hr;
	hr = ShaderClass::CreatePixelShader((WCHAR*)L"Core/Shaders/ColourGradientPS.cso", &m_ColourGradientPS);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT SkyColourGradient::InitialiseBuffers()
{
	auto device = ApplicationNew::Get().GetDevice();
	HRESULT hr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(GradientValuesBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = device->CreateBuffer(&bd, nullptr, &m_GradientValuesBufferPtr);
	if (FAILED(hr))
		return hr;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MatrixBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &m_MatrixBufferPtr);

	return S_OK;
}

void SkyColourGradient::InitialiseSkydomeElement()
{
	ObjectMaterial matte;
	matte.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	matte.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	matte.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	auto skyDomeTransform = std::make_shared<Transform>();
	skyDomeTransform->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	skyDomeTransform->SetScale(XMFLOAT3(1.0f, 0.5f, 1.0f));
	skyDomeTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

	NewObjectMesh sphere;
	ModelLoader::LoadModel(ApplicationNew::Get().GetDevice().Get(), L"Core/Resources/Objects/spherex1.obj", sphere, false);

	auto appearance = std::make_shared<Appearance>(sphere, matte);
	appearance->SetColourTexture(nullptr);

	m_SkyDomeElement = new SceneElement(GetStringHash("Sky dome"), *skyDomeTransform, *appearance);
	m_SkyDomeElement->SetCastShadows(false);
	m_SkyDomeElement->SetAffectedByLight(false);
}

void SkyColourGradient::SetAsCurrentShader()
{
	ShaderClass::SetShadersAndInputLayout(m_ColourGradientVS, m_ColourGradientPS, m_InputLayout);
}

void SkyColourGradient::Update(double deltaTime)
{
	m_SkyDomeElement->GetTransform()->SetPosition(m_SceneCentre);
	m_SkyDomeElement->Update(deltaTime);
}

void SkyColourGradient::Render(Camera& camera, XMFLOAT3&  sunPos)
{
	auto context = ApplicationNew::Get().GetContext();

	RenderClass::DisableZBuffer();
	RenderClass::SetRasterizerState(RasterizerType::NO_CULL);

	BufferClass::SetVertexShaderBuffers(&m_MatrixBufferPtr);
	BufferClass::SetPixelShaderBuffers(&m_GradientValuesBufferPtr);

	SetAsCurrentShader();

	GradientValuesBuffer colourValues;
	colourValues.HorizonColourDay = XMFLOAT4(0.7f, 0.80f, 0.92f, 1.0f);
	colourValues.OverheadColourDay = XMFLOAT4(0.17f, 0.26f, 0.69f, 1.0f);

	colourValues.HorizonColourSunrise = XMFLOAT4(0.89f, 0.59f, 0.27f, 1.0f);
	colourValues.OverheadColourSunrise = XMFLOAT4(0.29f, 0.38f, 0.58f, 1.0f);

	colourValues.HorizonColourNight = XMFLOAT4(0.01f, 0.02f, 0.04f, 1.0f);
	colourValues.OverheadColourNight = XMFLOAT4(0.01f, 0.02f, 0.04f, 1.0f);

	colourValues.SunPos = sunPos;

	MatrixBuffer matrixBufferValues;
	
	XMMATRIX view = XMLoadFloat4x4(&camera.GetView());
	XMMATRIX proj = XMLoadFloat4x4(&camera.GetPerspectiveProj());
	matrixBufferValues.View = XMMatrixTranspose(view);
	matrixBufferValues.Projection = XMMatrixTranspose(proj);

	XMMATRIX world = XMLoadFloat4x4(&m_SkyDomeElement->GetTransform()->GetWorld());
	matrixBufferValues.World = XMMatrixTranspose(world);

	context->UpdateSubresource(m_MatrixBufferPtr, 0, nullptr, &matrixBufferValues, 0, 0);
	context->UpdateSubresource(m_GradientValuesBufferPtr, 0, nullptr, &colourValues, 0, 0);

	m_SkyDomeElement->Draw();
}