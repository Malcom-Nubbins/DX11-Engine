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
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/distortionMap.dds", nullptr, &_distortionMap);
	CreateDDSTextureFromFile(_d3dClass->GetDevice(), L"Core/Resources/snow.dds", nullptr, &_snowTexture);
}
