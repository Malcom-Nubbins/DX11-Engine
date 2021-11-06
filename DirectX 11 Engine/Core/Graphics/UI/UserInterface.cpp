#include "UserInterface.h"

#include "../../ApplicationNew.h"
#include "../../Handlers/System Handlers/WindowClass.h"
#include "../../Loaders/ConfigLoader.h"
#include <vector>
#include <codecvt>

UserInterface::UserInterface(Camera& camera)
	: _camera(camera), _inputLayout(nullptr), _vertexShader(nullptr),
	  _pixelShader(nullptr), _matrixBuffer(nullptr)
{
	XMStoreFloat4x4(&_worldMatrix, XMMatrixIdentity());
}

UserInterface::~UserInterface()
{
}

void UserInterface::ReloadUI()
{
	for (auto uiElement : _bitmaps)
	{
		uiElement->Cleanup();
		delete uiElement;
	}

	_bitmaps.clear();

	LoadUIFromConfig();
}

void UserInterface::Cleanup()
{
	for (auto uiBitmap : _bitmaps)
	{
		uiBitmap->Cleanup();
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

	ShaderClass::CreateVertexShader((WCHAR*)L"Core/Shaders/UIVertexShader.cso", &_vertexShader, &_inputLayout, quadLayout, ARRAYSIZE(quadLayout));
	ShaderClass::CreatePixelShader((WCHAR*)L"Core/Shaders/UIPixelShader.cso", &_pixelShader);

	LoadUIFromConfig();
}

void UserInterface::AddBitmapToUI(S_UIElementInfo const& inElementInfo)
{
	auto window = ApplicationNew::Get().GetWindowByName(L"DX11 Engine");

	auto screenSize = XMFLOAT2(window->GetWindowWidth(), window->GetWindowHeight());
	UIBitmap* bitmap = new UIBitmap();
	bitmap->Initialise(screenSize, inElementInfo);

	if (bitmap->GetName() == GetStringHash("GrassyTest"))
	{
		bitmap->SetIsDynamicPos(true);
	}

	_bitmaps.push_back(bitmap);
}

UIBitmap* UserInterface::GetUIElement(StringHash const elementNameHash) const
{
	auto const it = std::find_if(_bitmaps.cbegin(), _bitmaps.cend(), [elementNameHash](UIBitmap const* element)
		{
			return element->GetName() == elementNameHash;
		});

	if (it != _bitmaps.cend())
	{
		return (*it);
	}
}

void UserInterface::Update(double delta)
{
	for(auto bitmap : _bitmaps)
	{
		bitmap->Update(delta);
	}
}

void UserInterface::Draw()
{
	auto app = &ApplicationNew::Get();

	RenderClass::SetRenderTargetAndDepthStencil(app->GetWindowByName(L"DX11 Engine")->GetBackBuffer(), nullptr);

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

void UserInterface::LoadUIFromConfig()
{
	using namespace rapidxml;
	using namespace std;
	auto configLoader = ApplicationNew::Get().GetConfigLoader();
	
	S_ConfigInfo const uiConfig = configLoader->GetConfigByName("UIConfig");
	if (!uiConfig.m_ConfigFilename.empty())
	{
		wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		string configFileName(converter.to_bytes(uiConfig.m_ConfigFilename));
		string filePath(configLoader->GetSettingStringValue(SettingType::Engine, "ConfigDir"));

		string const fullPath(filePath + configFileName);

		file<> xmlFile(fullPath.c_str());
		xml_document<> doc;

		doc.parse<0>(xmlFile.data());

		xml_node<>* rootNode = doc.first_node("UIConfig");

		for (xml_node<>* uiElementNode = rootNode->first_node("UIElement"); uiElementNode; uiElementNode = uiElementNode->next_sibling())
		{
			std::string const elementName(uiElementNode->first_attribute("name")->value());
			std::string textureName(uiElementNode->first_node("Texture")->value());
			float const texWidth = strtof(uiElementNode->first_node("Width")->value(), nullptr);
			float const texHeight = strtof(uiElementNode->first_node("Height")->value(), nullptr);

			float const offsetX = strtof(uiElementNode->first_node("OffsetX")->value(), nullptr);
			float const offsetY = strtof(uiElementNode->first_node("OffsetY")->value(), nullptr);
			
			std::string anchorPointStr(uiElementNode->first_node("Anchor")->value());
			UIAnchorPoint const anchorPoint(GetAnchorEnumValueFromString(anchorPointStr));

			std::string originPointStr(uiElementNode->first_node("Origin")->value());
			UIOriginPoint const originPoint(GetOriginEnumValueFromString(originPointStr));
			
			float const order = strtof(uiElementNode->first_node("Order")->value(), nullptr);

			S_UIElementInfo const elementInfo(elementName, XMFLOAT2(texWidth, texHeight), XMFLOAT2(offsetX, offsetY), anchorPoint, originPoint, textureName, static_cast<u32>(order));

			AddBitmapToUI(elementInfo);
		}

		std::sort(_bitmaps.begin(), _bitmaps.end(), [](UIBitmap* bitmapA, UIBitmap* bitmapB)
			{
				return bitmapA->GetOrder() > bitmapB->GetOrder();
			});
	}
}
