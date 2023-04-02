#include "UserInterface.h"

#include "../../ApplicationNew.h"
#include "../../Handlers/System Handlers/WindowClass.h"
#include "../../Loaders/ConfigLoader.h"
#include "../Core/Loaders/XMLLoader/rapidxml.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_ext.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_print.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_utils.hpp"
#include <vector>
#include <codecvt>

UserInterface::UserInterface(Camera& camera)
	: IConfigInterface(UI_CONFIG)
	, _camera(camera)
	, _inputLayout(nullptr)
	, _vertexShader(nullptr)
	, _pixelShader(nullptr)
	, _matrixBuffer(nullptr)
{
	XMStoreFloat4x4(&_worldMatrix, XMMatrixIdentity());
	LoadConfig();
}

UserInterface::~UserInterface()
{
	
}

void UserInterface::CreateConfig()
{
	using namespace std;
	using namespace rapidxml;

	ofstream newConfig(GetConfigName());
	xml_document<> doc;
	xml_node<>* decl = doc.allocate_node(node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "UTF-8"));
	doc.append_node(decl);

	xml_node<>* root = doc.allocate_node(node_element, "UIConfig");
	doc.append_node(root);

	xml_node<>* defaultUIElement = doc.allocate_node(node_element, "UIElement");
	defaultUIElement->append_attribute(doc.allocate_attribute("name", "Crosshair"));
	root->append_node(defaultUIElement);

	// Default UI element details
	{
		xml_node<>* texture = doc.allocate_node(node_element, "Texture");
		texture->value("Crosshair");
		defaultUIElement->append_node(texture);

		xml_node<>* width = doc.allocate_node(node_element, "Width");
		width->value("64");
		defaultUIElement->append_node(width);

		xml_node<>* height = doc.allocate_node(node_element, "Height");
		height->value("64");
		defaultUIElement->append_node(height);

		xml_node<>* anchor = doc.allocate_node(node_element, "Anchor");
		anchor->value("Centre");
		defaultUIElement->append_node(anchor);

		xml_node<>* origin = doc.allocate_node(node_element, "Origin");
		origin->value("Centre");
		defaultUIElement->append_node(origin);

		xml_node<>* offsetX = doc.allocate_node(node_element, "OffsetX");
		offsetX->value("0.0");
		defaultUIElement->append_node(offsetX);

		xml_node<>* offsetY = doc.allocate_node(node_element, "OffsetY");
		offsetY->value("0.0");
		defaultUIElement->append_node(offsetY);

		xml_node<>* order = doc.allocate_node(node_element, "Order");
		order->value("0");
		defaultUIElement->append_node(order);
	}

	newConfig << doc;

	newConfig.close();
	doc.clear();
}

void UserInterface::LoadConfig()
{
	using namespace std;
	using namespace rapidxml;
	
	if (!DoesConfigExist())
	{
		CreateConfig();
	}

	file<> xmlFile(UI_CONFIG);
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

void UserInterface::ReloadUI()
{
	for (auto uiElement : _bitmaps)
	{
		uiElement->Cleanup();
		delete uiElement;
	}

	_bitmaps.clear();

	LoadConfig();
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

	return nullptr;
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
