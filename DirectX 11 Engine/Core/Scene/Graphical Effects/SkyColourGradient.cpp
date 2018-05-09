#include "SkyColourGradient.h"

SkyColourGradient::SkyColourGradient(D3DClass * d3dClass, RenderClass * renderClass, ShaderClass * shaderClass, BufferClass * bufferClass)
	: _d3dClass(d3dClass), _shaderClass(shaderClass), _renderClass(renderClass), _bufferClass(bufferClass)
{
	_sceneCentre = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_colourGradientVS = nullptr;
	_colourGradientPS = nullptr;
	_inputLayout = nullptr;
	_noCullRasterizer = nullptr;
	_gradientValuesBuffer = nullptr;
}

SkyColourGradient::~SkyColourGradient()
{
}

void SkyColourGradient::Cleanup()
{
	_colourGradientVS->Release();
	_colourGradientPS->Release();
	_inputLayout->Release();
	_noCullRasterizer->Release();
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

	hr = _shaderClass->CreateVertexShader((WCHAR*)L"Core/Shaders/ColourGradientVS.hlsl", &_colourGradientVS , &_inputLayout, layout, ARRAYSIZE(layout));
	if (FAILED(hr))
		return hr;
	hr = _shaderClass->CreatePixelShader((WCHAR*)L"Core/Shaders/ColourGradientPS.hlsl", &_colourGradientPS);
	if (FAILED(hr))
		return hr;

	D3D11_RASTERIZER_DESC rasteriserdesc;
	ZeroMemory(&rasteriserdesc, sizeof(D3D11_RASTERIZER_DESC));
	rasteriserdesc.FillMode = D3D11_FILL_SOLID;
	rasteriserdesc.CullMode = D3D11_CULL_NONE;
	rasteriserdesc.MultisampleEnable = false;
	rasteriserdesc.AntialiasedLineEnable = false;
	rasteriserdesc.FrontCounterClockwise = false;
	hr = _d3dClass->GetDevice()->CreateRasterizerState(&rasteriserdesc, &_noCullRasterizer);
	if (FAILED(hr))
		return hr;

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = _d3dClass->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &_depthDisabled);
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
	hr = _d3dClass->GetDevice()->CreateBuffer(&bd, nullptr, &_gradientValuesBuffer);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void SkyColourGradient::InitialiseSkydomeElement()
{
	ObjectMaterial matte;
	matte.ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	matte.diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	matte.specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	_skyDomeElement = new SceneElement("Sky dome", OBJLoader::Load((char*)"Core/Resources/Objects/spherex1.obj", _d3dClass->GetDevice(), false), matte);
	_skyDomeElement->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	_skyDomeElement->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	_skyDomeElement->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	_skyDomeElement->SetColourTexture(nullptr);
	_skyDomeElement->SetCastShadows(false);
	_skyDomeElement->SetAffectedByLight(false);
}

void SkyColourGradient::SetAsCurrentShader()
{
	_shaderClass->SetShadersAndInputLayout(_colourGradientVS, _colourGradientPS, _inputLayout);
}

void SkyColourGradient::Update(float deltaTime)
{
	_skyDomeElement->SetPosition(_sceneCentre);
	_skyDomeElement->Update(deltaTime);
}

void SkyColourGradient::Render(ID3D11Buffer * matrixBuffer, const Camera& camera, const XMFLOAT3& sunPos)
{
	_d3dClass->GetContext()->OMSetDepthStencilState(_depthDisabled, 0);

	_d3dClass->GetContext()->RSSetState(_noCullRasterizer);

	_bufferClass->SetVertexShaderBuffers(&matrixBuffer);
	_bufferClass->SetPixelShaderBuffers(&_gradientValuesBuffer);

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
	XMMATRIX proj = XMLoadFloat4x4(&camera.GetProj());
	matrixBufferValues.View = XMMatrixTranspose(view);
	matrixBufferValues.Projection = XMMatrixTranspose(proj);

	XMMATRIX world = XMLoadFloat4x4(&_skyDomeElement->GetWorld());
	matrixBufferValues.World = XMMatrixTranspose(world);

	_d3dClass->GetContext()->UpdateSubresource(matrixBuffer, 0, nullptr, &matrixBufferValues, 0, 0);
	_d3dClass->GetContext()->UpdateSubresource(_gradientValuesBuffer, 0, nullptr, &colourValues, 0, 0);

	_skyDomeElement->Draw(_d3dClass->GetContext());
}