#include "TextureHandler.h"

TextureHandler::TextureHandler(D3DClass* d3dClass) : _d3dClass(d3dClass), _groundDisplacementMap(nullptr),
                                                     _groundSpecularMap(nullptr), _stoneTex(nullptr),
                                                     _stoneBumpMap(nullptr), _stoneDisplacementMap(nullptr),
                                                     _distortionMap(nullptr), _snowTexture(nullptr),
                                                     _aircraftTexture(nullptr), _aircraftNormalMap(nullptr),
                                                     _aircraftSpecularMap(nullptr), _aircraftDisplacementMap(nullptr)
{
	_groundColourTex = nullptr;
	_groundNormalMap = nullptr;
}

TextureHandler::~TextureHandler()
= default;

void TextureHandler::Cleanup()
{
	_groundColourTex->Release();
	_groundNormalMap->Release();
}

void TextureHandler::LoadAllTextures()
{
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/stone.dds", nullptr, &_groundColourTex);
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/stone_NRM.dds", nullptr, &_groundNormalMap);
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/stone_DISP.dds", nullptr, &_groundDisplacementMap);
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/stone_SPEC.dds", nullptr, &_groundSpecularMap);

	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/concrete_floor.dds", nullptr, &_stoneTex);
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/concrete_floor_NRM.dds", nullptr, &_stoneBumpMap);
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/concrete_floor_DISP.dds", nullptr, &_stoneDisplacementMap);
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/concrete_floor_SPEC.dds", nullptr, &_stoneSpecularMap);

	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/distortionMap.dds", nullptr, &_distortionMap);
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/snow.dds", nullptr, &_snowTexture);

	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/Hercules_COLOR.dds", nullptr, &_aircraftTexture);
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/Hercules_NRM_new.dds", nullptr, &_aircraftNormalMap);
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/Hercules_SPEC_new.dds", nullptr, &_aircraftSpecularMap);
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/Hercules_DISP_new.dds", nullptr, &_aircraftDisplacementMap);

	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/metal_floor.dds", nullptr, &_metalFloorColourTex);
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/metal_floor_NRM.dds", nullptr, &_metalFloorNormalMap);
}
