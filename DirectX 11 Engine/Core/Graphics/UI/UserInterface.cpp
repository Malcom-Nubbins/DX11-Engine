#include "UserInterface.h"

UserInterface::UserInterface(SystemHandlers* systemHandlers, Camera* camera)
	: _systemHandlers(systemHandlers), _camera(camera)
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
	_systemHandlers->GetD3DClass()->GetDevice()->CreateBuffer(&bd, nullptr, &_matrixBuffer);

	D3D11_INPUT_ELEMENT_DESC quadLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	_systemHandlers->GetShaderClass()->CreateVertexShader((WCHAR*)L"Core/Shaders/UIVertexShader.hlsl", &_vertexShader, &_inputLayout, quadLayout, ARRAYSIZE(quadLayout));
	_systemHandlers->GetShaderClass()->CreatePixelShader((WCHAR*)L"Core/Shaders/FullscreenQuadPS.hlsl", &_pixelShader);
}

void UserInterface::AddBitmapToUI(XMFLOAT2 bitmapSize, XMFLOAT2 bitmapPos, ID3D11ShaderResourceView* bitmapTexture)
{
	auto screenSize = XMFLOAT2(_systemHandlers->GetWindowClass()->GetWindowWidth(), _systemHandlers->GetWindowClass()->GetWindowHeight());
	UIBitmap* bitmap = new UIBitmap(_systemHandlers->GetD3DClass(), _systemHandlers->GetBufferClass());
	bitmap->Initialise(screenSize, bitmapSize, bitmapTexture);
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
	_systemHandlers->GetRenderClass()->DisableZBuffer();
	_systemHandlers->GetRenderClass()->EnableAlphaBlending();

	_systemHandlers->GetD3DClass()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_systemHandlers->GetShaderClass()->SetShadersAndInputLayout(_vertexShader, _pixelShader, _inputLayout);

	_systemHandlers->GetBufferClass()->SetVertexShaderBuffers(&_matrixBuffer, 0);
	MatrixBuffer matBuffer;
	XMMATRIX view = XMMatrixTranspose(XMLoadFloat4x4(&_camera->GetDefaultView()));
	XMMATRIX proj = XMMatrixTranspose(XMLoadFloat4x4(&_camera->GetOthographicProj()));

	matBuffer.View = view;
	matBuffer.Projection = proj;

	_systemHandlers->GetD3DClass()->GetContext()->PSSetSamplers(0, 1, _systemHandlers->GetShaderClass()->GetSamplerState(LINEAR));

	for (auto bitmap : _bitmaps)
	{
		matBuffer.World = XMMatrixTranspose(XMLoadFloat4x4(&bitmap->GetUIElement()->GetTransform()->GetWorld()));
		_systemHandlers->GetD3DClass()->GetContext()->UpdateSubresource(_matrixBuffer, 0, nullptr, &matBuffer, 0, 0);
		bitmap->Draw();
	}

	_systemHandlers->GetRenderClass()->EnableZBuffer();
	_systemHandlers->GetRenderClass()->DisableAlphaBlending();
}
