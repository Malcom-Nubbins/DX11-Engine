#include "TextureHandler.h"

TextureHandler::TextureHandler(D3DClass* d3dClass) : _d3dClass(d3dClass)
{
	_groundColourTex = nullptr;
	_groundNormalMap = nullptr;
}

TextureHandler::~TextureHandler()
{
}

void TextureHandler::Cleanup()
{
	_groundColourTex->Release();
	_groundNormalMap->Release();
}

void TextureHandler::LoadAllTextures()
{
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/grassyStone.dds", nullptr, &_groundColourTex);
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/grassyStone_NRM.dds", nullptr, &_groundNormalMap);
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/grassyStone_DISP.dds", nullptr, &_groundDisplacementMap);

	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/tile_colour_tex.dds", nullptr, &_stoneTex);
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/tile_bump_map.dds", nullptr, &_stoneBumpMap);
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/stone_DISP.dds", nullptr, &_stoneDisplacementMap);

	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/distortionMap.dds", nullptr, &_distortionMap);
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/snow.dds", nullptr, &_snowTexture);

	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/Hercules_COLOR.dds", nullptr, &_aircraftTexture);
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/Hercules_NRM.dds", nullptr, &_aircraftNormalMap);
}
