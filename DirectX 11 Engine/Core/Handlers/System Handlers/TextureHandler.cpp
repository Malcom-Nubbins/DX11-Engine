#include "TextureHandler.h"

#include "../../ApplicationNew.h"

std::map<char const*, ID3D11ShaderResourceView*> TextureHandler::m_Textures;

TextureHandler::TextureHandler() : m_GroundDisplacementMap(nullptr),
                                                     m_GroundSpecularMap(nullptr), m_stoneTex(nullptr),
                                                     m_stoneBumpMap(nullptr), m_stoneDisplacementMap(nullptr),
                                                     m_distortionMap(nullptr), m_snowTexture(nullptr),
                                                     m_aircraftTexture(nullptr), m_aircraftNormalMap(nullptr),
                                                     m_aircraftSpecularMap(nullptr), m_aircraftDisplacementMap(nullptr)
{
}

TextureHandler::~TextureHandler()
= default;

void TextureHandler::Cleanup()
{
	for (auto tex : m_Textures)
	{
		tex.second->Release();
		tex.second = nullptr;
	}

	m_Textures.clear();
}

void TextureHandler::LoadAllTextures()
{
	ComPtr<ID3D11Device> const& device = ApplicationNew::Get().GetDevice();

	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/stone.dds", nullptr, &m_GroundColourTex);
	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/stone_NRM.dds", nullptr, &m_GroundNormalMap);
	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/stone_DISP.dds", nullptr, &m_GroundDisplacementMap);
	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/stone_SPEC.dds", nullptr, &m_GroundSpecularMap);

	m_Textures.emplace(std::make_pair("StoneColour", m_GroundColourTex));
	m_Textures.emplace(std::make_pair("StoneNormal", m_GroundNormalMap));
	m_Textures.emplace(std::make_pair("StoneDisplacement", m_GroundDisplacementMap));
	m_Textures.emplace(std::make_pair("StoneSpecular", m_GroundSpecularMap));

	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/concrete_floor_2.dds", nullptr, &m_stoneTex);
	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/concrete_floor_2_NRM.dds", nullptr, &m_stoneBumpMap);
	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/concrete_floor_2_DISP.dds", nullptr, &m_stoneDisplacementMap);
	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/concrete_floor_SPEC.dds", nullptr, &m_stoneSpecularMap);

	m_Textures.emplace(std::make_pair("ConcreteColour", m_stoneTex));
	m_Textures.emplace(std::make_pair("ConcreteNormal", m_stoneBumpMap));
	m_Textures.emplace(std::make_pair("ConcreteDisplacement", m_stoneDisplacementMap));
	m_Textures.emplace(std::make_pair("ConcreteSpecular", m_stoneSpecularMap));

	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/distortionMap.dds", nullptr, &m_distortionMap);
	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/snow.dds", nullptr, &m_snowTexture);

	m_Textures.emplace(std::make_pair("DistortionMap", m_distortionMap));
	m_Textures.emplace(std::make_pair("Snow", m_snowTexture));

	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/Hercules_COLOR.dds", nullptr, &m_aircraftTexture);
	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/Hercules_NRM_new.dds", nullptr, &m_aircraftNormalMap);
	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/Hercules_SPEC_new.dds", nullptr, &m_aircraftSpecularMap);
	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/Hercules_DISP_new.dds", nullptr, &m_aircraftDisplacementMap);

	m_Textures.emplace(std::make_pair("AircraftColour", m_aircraftTexture));
	m_Textures.emplace(std::make_pair("AircraftNormal", m_aircraftNormalMap));
	m_Textures.emplace(std::make_pair("AircraftSpecular", m_aircraftSpecularMap));
	m_Textures.emplace(std::make_pair("AircraftDisplacement", m_aircraftDisplacementMap));

	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/metal_floor.dds", nullptr, &m_metalFloorColourTex);
	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/metal_floor_NRM.dds", nullptr, &m_metalFloorNormalMap);

	m_Textures.emplace(std::make_pair("MetalColour", m_metalFloorColourTex));
	m_Textures.emplace(std::make_pair("MetalNormal", m_metalFloorNormalMap));

	ComPtr<ID3D11ShaderResourceView> grassyStoneTex = nullptr;
	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/grassyStone.dds", nullptr, grassyStoneTex.GetAddressOf());
	m_Textures.emplace(std::make_pair("GrassyStoneColour", std::move(grassyStoneTex.Get())));
	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/grassyStone_NRM.dds", nullptr, grassyStoneTex.GetAddressOf());
	m_Textures.emplace(std::make_pair("GrassyStoneNormal", std::move(grassyStoneTex.Get())));
	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/grassyStone_DISP.dds", nullptr, grassyStoneTex.GetAddressOf());
	m_Textures.emplace(std::make_pair("GrassyStoneDisplacement", std::move(grassyStoneTex.Get())));
	CreateDDSTextureFromFile(device.Get(), L"Core/Resources/grassyStone_SPEC.dds", nullptr, grassyStoneTex.GetAddressOf());
	m_Textures.emplace(std::make_pair("GrassyStoneSpecular", std::move(grassyStoneTex.Get())));
}

ID3D11ShaderResourceView* TextureHandler::GetTextureByName(char const* name)
{
	m_Textures[name]->AddRef();
	return m_Textures[name];
}
