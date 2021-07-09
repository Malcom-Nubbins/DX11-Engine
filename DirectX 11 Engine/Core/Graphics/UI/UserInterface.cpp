#include "UserInterface.h"

#include "../../ApplicationNew.h"
#include "../../Handlers/System Handlers/WindowClass.h"

UserInterface::UserInterface(Camera& camera)
	: _camera(camera), _inputLayout(nullptr), _vertexShader(nullptr),
	  _pixelShader(nullptr), _matrixBuffer(nullptr)
{
	XMStoreFloat4x4(&_worldMatrix, XMMatrixIdentity());
}

UserInterface::~UserInterface()
{
}

void UserInterface::Cleanup()
{
	for (auto uiBitmap : _bitmaps)
	{
		delete uiBitmap;
	}

	_bitmaps.clear();
	_bitmaps.shrink_to_fit();

	if (_inputLayout)
		_inputLayout->Release();

	if (_vertexShader)
		_vertexShader->Release();

	if (_pixelShader)
		_pixelShader->Release();

	if (_matrixBuffer)
		_matrixBuffer->Release();
}

void UserInterface::Resize(float width, float height)
{
	for (UIBitmap* bitmap : _bitmaps)
	{
		bitmap->UpdateScreenSize(XMFLOAT2(width, height));
	}
}

void UserInterface::Initialise()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MatrixBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	ApplicationNew::Get().GetDevice()->CreateBuffer(&bd, nullptr, &_matrixBuffer);

	D3D11_INPUT_ELEMENT_DESC quadLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	ShaderClass::CreateVertexShader((WCHAR*)L"Core/Shaders/UIVertexShader.hlsl", &_vertexShader, &_inputLayout, quadLayout, ARRAYSIZE(quadLayout));
	ShaderClass::CreatePixelShader((WCHAR*)L"Core/Shaders/FullscreenQuadPS.hlsl", &_pixelShader);
}

void UserInterface::AddBitmapToUI(XMFLOAT2 bitmapSize, XMFLOAT2 bitmapPos, ID3D11ShaderResourceView* bitmapTexture)
{
	auto window = ApplicationNew::Get().GetWindowByName(L"DX11 Engine");

	auto screenSize = XMFLOAT2(window->GetWindowWidth(), window->GetWindowHeight());
	UIBitmap* bitmap = new UIBitmap();
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
	auto app = &ApplicationNew::Get();

	RenderClass::DisableZBuffer();
	RenderClass::EnableAlphaBlending();

	app->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ShaderClass::SetShadersAndInputLayout(_vertexShader, _pixelShader, _inputLayout);

	BufferClass::SetVertexShaderBuffers(&_matrixBuffer, 0);
	MatrixBuffer matBuffer;
	XMMATRIX view = XMMatrixTranspose(XMLoadFloat4x4(&_camera.GetDefaultView()));
	XMMATRIX proj = XMMatrixTranspose(XMLoadFloat4x4(&_camera.GetOthographicProj()));

	matBuffer.View = view;
	matBuffer.Projection = proj;

	app->GetContext()->PSSetSamplers(0, 1, ShaderClass::GetSamplerState(LINEAR));

	for (auto bitmap : _bitmaps)
	{
		matBuffer.World = XMMatrixTranspose(XMLoadFloat4x4(&bitmap->GetUIElement()->GetTransform()->GetWorld()));
		app->GetContext()->UpdateSubresource(_matrixBuffer, 0, nullptr, &matBuffer, 0, 0);
		bitmap->Draw();
	}

	RenderClass::EnableZBuffer();
	RenderClass::DisableAlphaBlending();
}
