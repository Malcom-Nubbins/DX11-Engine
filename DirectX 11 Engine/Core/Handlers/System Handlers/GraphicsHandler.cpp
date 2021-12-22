#include "GraphicsHandler.h"

#include "../Core/ApplicationNew.h"
#include "../Core/Handlers/System Handlers/RenderClass.h"
#include "../Core/Handlers/System Handlers/InputHandler.h"
#include "../Core/Scene/SceneHandler.h"
#include "../Core/Scene/Scene.h"
#include "../Core/DX11Engine.h"
#include "../Core/Loaders/ConfigLoader.h"
#include "../Core/Loaders/XMLLoader/rapidxml.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_ext.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_print.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_utils.hpp"
#include <vector>
#include <codecvt>

GraphicsHandler::GraphicsHandler() 
	: _basicLight(nullptr)
	, _shadows(nullptr)
	, _skyGradient(nullptr)
	, _renderToQuad(nullptr)
	, _heatHaze(nullptr)
	, _sceneLight()
	, _sceneFog()
	, _spotLight()
{
	_lightRotationAmount = XMConvertToRadians(00.0f);

	_fullRotationAmount = XMConvertToRadians(360.0f);
	_currentTime = XMConvertToRadians(90.0f);
	_currentDay = 1;
	_currentSeason = 0;

	_seasonsSunHeightOffset = std::vector<int>
	{
		{0},
		{2},
		{0},
		{-2}
	};

	_seasonNames = std::map<int, std::string>
	{
		{0, "Spring"},
		{1, "Summer"},
		{2, "Autumn"},
		{3, "Winter"}
	};
}

GraphicsHandler::~GraphicsHandler()
{
}

void GraphicsHandler::PreResizeViews()
{
	_basicLight->PreResize();
	_heatHaze->PreResize();
	_shadows->PreResizeViews();
	_renderToQuad->PreResize();
}

void GraphicsHandler::ResizeViews(float const windowWidth, float const windowHeight)
{
	_basicLight->Resize(windowWidth, windowHeight);
	_heatHaze->Resize(windowWidth, windowHeight);
	_shadows->OnResize(4096, 4096);
	_renderToQuad->OnResize(windowWidth, windowHeight);
}

void GraphicsHandler::Init(float const windowWidth, float const windowHeight)
{
	_basicLight = new BasicLight();
	_basicLight->Initialise(windowWidth, windowHeight);

	_shadows = new Shadows();
	_shadows->Initialise(4096, 4096);

	_skyGradient = new SkyColourGradient();
	_skyGradient->Initialise();

	_renderToQuad = new RenderToFullscreenQuad();
	_renderToQuad->Initialise(windowWidth, windowHeight);

	_heatHaze = new HeatHaze();
	_heatHaze->Initialise(windowWidth, windowHeight);

	_sceneLight.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	_sceneLight.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_sceneLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_sceneLight.lightDirection = XMFLOAT3(5.0f, 5.0f, 0.0f);

	_preOffsetLightDir = _sceneLight.lightDirection;

	_sceneFog.FogStart = 5.0f;
	_sceneFog.FogRange = 50.0f;
	_sceneFog.FogColourDay = XMFLOAT4(0.7f, 0.80f, 0.92f, 1.0f);
	_sceneFog.FogColourNight = XMFLOAT4(0.01f, 0.02f, 0.04f, 1.0f);
	_sceneFog.FogColourSunriseSunset = XMFLOAT4(0.89f, 0.59f, 0.27f, 1.0f);
	_sceneFog.UseFog = 1.0f;

	_spotLight.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	_spotLight.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_spotLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_spotLight.attenuation = XMFLOAT3(0.4f, 0.02f, 0.0f);
	_spotLight.spot = 20.0f;
	_spotLight.range = 1000.0f;

	LoadMaterialsConfig();
}

void GraphicsHandler::Cleanup()
{
	_heatHaze->Cleanup();
	delete _heatHaze;
	_heatHaze = nullptr;

	_skyGradient->Cleanup();
	delete _skyGradient;
	_skyGradient = nullptr;

	_shadows->Cleanup();
	delete _shadows;
	_shadows = nullptr;

	_basicLight->Cleanup();
	delete _basicLight;
	_basicLight = nullptr;

	_renderToQuad->Cleanup();
	delete _renderToQuad;
	_renderToQuad = nullptr;
}

void GraphicsHandler::Update(UpdateEvent& e)
{
	XMFLOAT3 lightDir = _preOffsetLightDir;

	// Rotate the light at a 45 degree angle
	XMFLOAT3 rotationAxis = XMFLOAT3(0.0f, -0.5f, 0.5f);
	XMVECTOR axis = XMLoadFloat3(&rotationAxis);
	XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, _lightRotationAmount * static_cast<float>(e.ElapsedTime));

	XMVECTOR rotatedLight = XMVector3Rotate(XMLoadFloat3(&lightDir), rotationQuaternion);
	XMStoreFloat3(&lightDir, rotatedLight);
	_preOffsetLightDir = lightDir;

	_currentTime += _lightRotationAmount * static_cast<float>(e.ElapsedTime);
	if (_currentTime >= _fullRotationAmount)
	{
		// Day has passed
		_currentDay++;
		if (_currentDay > 16)
			_currentDay = 1;

		_currentTime = 0.0f;

		if (_currentDay % 4 == 0)
		{
			// Season has passed
			_currentSeason++;
			if (_currentSeason > 3)
				_currentSeason = 0;
		}
	}
	else if (_currentTime < 0.0f)
	{
		_currentDay--;
		if (_currentDay < 1)
			_currentDay = 16;
		_currentTime = _fullRotationAmount;

		if (_currentDay % 4 == 0)
		{
			// Season has passed
			_currentSeason--;
			if (_currentSeason < 0)
				_currentSeason = 3;
		}
	}

	// Raise or lower the height of the sun
	lightDir.y += _seasonsSunHeightOffset[_currentSeason];

	if (_seasonNames.find(_currentSeason)->second == "Spring" || _seasonNames.find(_currentSeason)->second == "Autumn")
	{
		_sceneFog.FogStart = 13.0f;
		_sceneFog.FogRange = 150.0f;
	}
	else if (_seasonNames.find(_currentSeason)->second == "Summer")
	{
		_sceneFog.FogStart = 24.0f;
		_sceneFog.FogRange = 240.0f;
	}
	else if (_seasonNames.find(_currentSeason)->second == "Winter")
	{
		_sceneFog.FogStart = 7.0f;
		_sceneFog.FogRange = 80.0f;
	}

	if (InputHandler::IsKeyDown(Keys::OemPeriod))
	{
		_lightRotationAmount += (0.5f * static_cast<float>(e.ElapsedTime));

		if (_lightRotationAmount > 2.0f)
			_lightRotationAmount = 2.0f;
	}

	if (InputHandler::IsKeyDown(Keys::OemComma))
	{
		_lightRotationAmount -= (0.5f * static_cast<float>(e.ElapsedTime));

		if (_lightRotationAmount < -2.0f)
			_lightRotationAmount = -2.0f;
	}

	if (InputHandler::IsKeyDown(Keys::D1))
	{
		// Switch to Spring
		_currentSeason = 0;
		_currentDay = 1;

	}
	if (InputHandler::IsKeyDown(Keys::D2))
	{
		// Switch to Summer
		_currentSeason = 1;
		_currentDay = 5;
	}
	if (InputHandler::IsKeyDown(Keys::D3))
	{
		// Switch to Autumn
		_currentSeason = 2;
		_currentDay = 9;
	}
	if (InputHandler::IsKeyDown(Keys::D4))
	{
		// Switch to Winter
		_currentSeason = 3;
		_currentDay = 13;
	}

	if (InputHandler::IsKeyDown(Keys::T))
	{
		_lightRotationAmount = 0.0f;
	}

	_sceneLight.lightDirection = lightDir;
	_shadows->UpdateLightDirection(_sceneLight.lightDirection);

	Scene* currScene = DX11Engine::Get().GetSceneHandler()->GetCurrentScene();
	Player const* currPlayer = DX11Engine::Get().GetPlayer();

	_shadows->SetSceneCentre(currPlayer->GetPlayerPosition());
	_shadows->BuildShadowTransform();

	for (SceneElement* element : currScene->GetAllSceneElements())
	{
		if (element->GetElementName() == GetStringHash("Light Source Sphere"))
		{
			element->GetTransform()->SetPosition(_shadows->GetLightPosition());
		}
		element->Update(e.ElapsedTime);
	}

	_skyGradient->SetSceneCentre(currPlayer->GetPlayerPosition());
	_skyGradient->Update(e.ElapsedTime);

	_basicLight->CalculateLightColour(_sceneLight, _shadows->GetLightPosition().y, _sceneFog);

	_heatHaze->Update(static_cast<float>(e.TotalTime / 2.0));

	if (InputHandler::IsKeyDown(Keys::LeftControl) && InputHandler::IsKeyUp(Keys::R))
	{
		_basicLight->SetWireframeMode(!_basicLight->GetWireframeState());
	}

	_spotLight.position = currPlayer->GetPlayerPosition();
	_spotLight.direction = currPlayer->GetPlayerLookDirection();
}

void GraphicsHandler::Draw()
{
	Scene* currScene = DX11Engine::Get().GetSceneHandler()->GetCurrentScene();
	Player const* currPlayer = DX11Engine::Get().GetPlayer();

	XMFLOAT3 sunPos = _shadows->GetLightPosition();
	_sceneFog.sunHeight = sunPos.y;

	_basicLight->SetAsCurrentRenderTarget();
	_basicLight->SetAsCurrentViewport();
	RenderClass::DisableRtvClearing();

	_skyGradient->Render(currPlayer->GetCamera(), sunPos);
	RenderClass::EnableZBuffer();
	_shadows->Render(currScene->GetAllSceneElements());

	_basicLight->Render(currPlayer->GetCamera(), _sceneLight, _pointLights, _spotLight, _sceneFog, currScene->GetAllSceneElements(), *_shadows);

	_renderToQuad->SetAsCurrentVertexShader();

	if (_seasonNames.find(_currentSeason)->second == "Summer" || _seasonNames.find(_currentSeason)->second == "Winter")
	{
		_heatHaze->Render(_basicLight->GetRenderTargetSRV(), _seasonNames.find(_currentSeason)->second);
		_renderToQuad->Render(_heatHaze->GetTexture());
	}
	else
	{
		_renderToQuad->Render(_basicLight->GetRenderTargetSRV());
	}
}

ObjectMaterial GraphicsHandler::GetMaterialByName(char const* name) const
{
	std::string matName(name);

	auto const it = std::find_if(m_AllMaterials.cbegin(), m_AllMaterials.cend(), [matName](S_Material const& material)
		{
			return material.m_MaterialName == matName;
		});

	if (it != m_AllMaterials.cend())
	{
		return (*it).m_Material;
	}

	return ObjectMaterial();
}

void GraphicsHandler::LoadMaterialsConfig()
{
	auto configLoader = ApplicationNew::Get().GetConfigLoader();
	std::vector<S_MaterialInfo> const allMaterials = configLoader->GetAllMaterials();
	assert(!allMaterials.empty());

	for (auto const& matInfo : allMaterials)
	{
		S_Material material(matInfo.m_MaterialName.c_str(), ObjectMaterial(matInfo.m_MaterialAmbient, matInfo.m_MaterialDiffuse, matInfo.m_MaterialSpecular));
		
		m_AllMaterials.emplace_back(std::move(material));
	}
}

//void GraphicsHandler::CreateDefaultMaterialsConfig(std::string const& inConfigFilename)
//{
//}
