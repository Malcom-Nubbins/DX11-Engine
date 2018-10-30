#include "UserInterface.h"


UserInterface::UserInterface(D3DClass* d3dClass, ShaderClass* shaderClass, RenderClass* renderClass,
	BufferClass* bufferClass, WindowClass* windowClass, Camera* camera)
	: _d3dClass(d3dClass), _renderClass(renderClass), _shaderClass(shaderClass), _bufferClass(bufferClass), _windowClass(windowClass),
	  _camera(camera)
{
	XMStoreFloat4x4(&_worldMatrix, XMMatrixIdentity());
}

UserInterface::~UserInterface()
{
}

void UserInterface::Initialise()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MatrixBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	_d3dClass->GetDevice()->CreateBuffer(&bd, nullptr, &_matrixBuffer);

	D3D11_INPUT_ELEMENT_DESC quadLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	_shaderClass->CreateVertexShader((WCHAR*)L"Core/Shaders/UIVertexShader.hlsl", &_vertexShader, &_inputLayout, quadLayout, ARRAYSIZE(quadLayout));
	_shaderClass->CreatePixelShader((WCHAR*)L"Core/Shaders/FullscreenQuadPS.hlsl", &_pixelShader);
}

void UserInterface::AddBitmapToUI(XMFLOAT2 bitmapSize, XMFLOAT2 bitmapPos, ID3D11ShaderResourceView* bitmapTexture)
{
	UIBitmap* bitmap = new UIBitmap(_d3dClass, _bufferClass);
	bitmap->Initialise(XMFLOAT2(_windowClass->GetWindowWidth(), _windowClass->GetWindowHeight()), bitmapSize, bitmapTexture);
	bitmap->MoveBitmap(bitmapPos);

	_bitmaps.push_back(bitmap);
}

void UserInterface::Update(float delta)
{
	for(auto bitmap : _bitmaps)
	{
		bitmap->Update(delta);
	}
}

void UserInterface::Draw()
{
	_renderClass->DisableZBuffer();
	_renderClass->EnableAlphaBlending();

	_d3dClass->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_shaderClass->SetShadersAndInputLayout(_vertexShader, _pixelShader, _inputLayout);

	_bufferClass->SetVertexShaderBuffers(&_matrixBuffer, 0);
	MatrixBuffer matBuffer;
	XMMATRIX view = XMMatrixTranspose(XMLoadFloat4x4(&_camera->GetDefaultView()));
	XMMATRIX proj = XMMatrixTranspose(XMLoadFloat4x4(&_camera->GetOthographicProj()));

	matBuffer.View = view;
	matBuffer.Projection = proj;

	_d3dClass->GetContext()->PSSetSamplers(0, 1, _shaderClass->GetSamplerState(LINEAR));

	for (auto bitmap : _bitmaps)
	{
		matBuffer.World = XMMatrixTranspose(XMLoadFloat4x4(&bitmap->GetUIElement()->GetTransform()->GetWorld()));
		_d3dClass->GetContext()->UpdateSubresource(_matrixBuffer, 0, nullptr, &matBuffer, 0, 0);
		bitmap->Draw();
	}

	_renderClass->EnableZBuffer();
	_renderClass->DisableAlphaBlending();
}
