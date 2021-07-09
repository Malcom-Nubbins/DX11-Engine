#include "TextureHandler.h"

std::map<char const*, ID3D11ShaderResourceView*> TextureHandler::m_Textures;

TextureHandler::TextureHandler(ID3D11Device* device, ID3D11DeviceContext* context) : m_Device(device), m_Context(context), m_GroundDisplacementMap(nullptr),
                                                     m_GroundSpecularMap(nullptr), m_stoneTex(nullptr),
                                                     m_stoneBumpMap(nullptr), m_stoneDisplacementMap(nullptr),
                                                     m_distortionMap(nullptr), m_snowTexture(nullptr),
                                                     m_aircraftTexture(nullptr), m_aircraftNormalMap(nullptr),
                                                     m_aircraftSpecularMap(nullptr), m_aircraftDisplacementMap(nullptr)
{
	m_GroundColourTex = nullptr;
	m_GroundNormalMap = nullptr;
}

TextureHandler::~TextureHandler()
= default;

void TextureHandler::Cleanup()
{
	m_Textures.clear();

	m_GroundColourTex->Release();
	m_GroundNormalMap->Release();
}

void TextureHandler::LoadAllTextures()
{
	CreateDDSTextureFromFile(m_Device.Get(), L"Core/Resources/stone.dds", nullptr, &m_GroundColourTex);
	CreateDDSTextureFromFile(m_Device.Get(), L"Core/Resources/stone_NRM.dds", nullptr, &m_GroundNormalMap);
	CreateDDSTextureFromFile(m_Device.Get(), L"Core/Resources/stone_DISP.dds", nullptr, &m_GroundDisplacementMap);
	CreateDDSTextureFromFile(m_Device.Get(), L"Core/Resources/stone_SPEC.dds", nullptr, &m_GroundSpecularMap);

	m_Textures.emplace(std::make_pair("StoneColour", m_GroundColourTex));
	m_Textures.emplace(std::make_pair("StoneNormal", m_GroundNormalMap));
	m_Textures.emplace(std::make_pair("StoneDisplacement", m_GroundDisplacementMap));
	m_Textures.emplace(std::make_pair("StoneSpecular", m_GroundSpecularMap));

	CreateDDSTextureFromFile(m_Device.Get(), L"Core/Resources/concrete_floor_2.dds", nullptr, &m_stoneTex);
	CreateDDSTextureFromFile(m_Device.Get(), L"Core/Resources/concrete_floor_2_NRM.dds", nullptr, &m_stoneBumpMap);
	CreateDDSTextureFromFile(m_Device.Get(), L"Core/Resources/concrete_floor_2_DISP.dds", nullptr, &m_stoneDisplacementMap);
	CreateDDSTextureFromFile(m_Device.Get(), L"Core/Resources/concrete_floor_SPEC.dds", nullptr, &m_stoneSpecularMap);

	m_Textures.emplace(std::make_pair("ConcreteColour", m_stoneTex));
	m_Textures.emplace(std::make_pair("ConcreteNormal", m_stoneBumpMap));
	m_Textures.emplace(std::make_pair("ConcreteDisplacement", m_stoneDisplacementMap));
	m_Textures.emplace(std::make_pair("ConcreteSpecular", m_stoneSpecularMap));

	CreateDDSTextureFromFile(m_Device.Get(), L"Core/Resources/distortionMap.dds", nullptr, &m_distortionMap);
	CreateDDSTextureFromFile(m_Device.Get(), L"Core/Resources/snow.dds", nullptr, &m_snowTexture);

	m_Textures.emplace(std::make_pair("DistortionMap", m_distortionMap));
	m_Textures.emplace(std::make_pair("Snow", m_snowTexture));

	CreateDDSTextureFromFile(m_Device.Get(), L"Core/Resources/Hercules_COLOR.dds", nullptr, &m_aircraftTexture);
	CreateDDSTextureFromFile(m_Device.Get(), L"Core/Resources/Hercules_NRM_new.dds", nullptr, &m_aircraftNormalMap);
	CreateDDSTextureFromFile(m_Device.Get(), L"Core/Resources/Hercules_SPEC_new.dds", nullptr, &m_aircraftSpecularMap);
	CreateDDSTextureFromFile(m_Device.Get(), L"Core/Resources/Hercules_DISP_new.dds", nullptr, &m_aircraftDisplacementMap);

	m_Textures.emplace(std::make_pair("AircraftColour", m_aircraftTexture));
	m_Textures.emplace(std::make_pair("AircraftNormal", m_aircraftNormalMap));
	m_Textures.emplace(std::make_pair("AircraftSpecular", m_aircraftSpecularMap));
	m_Textures.emplace(std::make_pair("AircraftDisplacement", m_aircraftDisplacementMap));

	CreateDDSTextureFromFile(m_Device.Get(), L"Core/Resources/metal_floor.dds", nullptr, &m_metalFloorColourTex);
	CreateDDSTextureFromFile(m_Device.Get(), L"Core/Resources/metal_floor_NRM.dds", nullptr, &m_metalFloorNormalMap);

	m_Textures.emplace(std::make_pair("MetalColour", m_metalFloorColourTex));
	m_Textures.emplace(std::make_pair("MetalNormal", m_metalFloorNormalMap));
}

ID3D11ShaderResourceView* TextureHandler::GetTextureByName(char const* name)
{
	return m_Textures[name];
}
