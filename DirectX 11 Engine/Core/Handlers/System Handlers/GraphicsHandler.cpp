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

using namespace DirectX;

GraphicsHandler::GraphicsHandler() 
	: m_BasicLight(nullptr)
	, m_Shadows(nullptr)
	, m_SkyGradient(nullptr)
	, m_RenderToQuad(nullptr)
	, m_HeatHaze(nullptr)
	, m_SceneLight()
	, m_SceneFog()
	, m_SpotLight()
{
	m_LightRotationAmount = XMConvertToRadians(00.0f);

	m_FullRotationAmount = XMConvertToRadians(360.0f);
	m_CurrentTime = XMConvertToRadians(90.0f);
	m_CurrentDay = 1;
	m_CurrentSeason = 0;

	m_SeasonsSunHeightOffset =
	{
		{0},
		{2},
		{0},
		{-2}
	};

	m_SeasonNames =
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
	m_BasicLight->PreResize();
	m_HeatHaze->PreResize();
	m_Shadows->PreResizeViews();
	m_RenderToQuad->PreResize();
}

void GraphicsHandler::ResizeViews(float const windowWidth, float const windowHeight)
{
	m_BasicLight->Resize(windowWidth, windowHeight);
	m_HeatHaze->Resize(windowWidth, windowHeight);

	int const shadowQuality = ApplicationNew::Get().GetConfigLoader()->GetSettingValue(SettingType::Graphics, "Shadows");

	m_Shadows->OnResize(static_cast<float>(shadowQuality), static_cast<float>(shadowQuality));
	m_RenderToQuad->OnResize(windowWidth, windowHeight);
}

void GraphicsHandler::Init(float const windowWidth, float const windowHeight)
{
	m_BasicLight = new BasicLight();
	m_BasicLight->Initialise(windowWidth, windowHeight);

	int const shadowQuality = ApplicationNew::Get().GetConfigLoader()->GetSettingValue(SettingType::Graphics, "Shadows");

	m_Shadows = new Shadows();
	m_Shadows->Initialise(static_cast<float>(shadowQuality), static_cast<float>(shadowQuality));

	m_SkyGradient = new SkyColourGradient();
	m_SkyGradient->Initialise();

	m_RenderToQuad = new RenderToFullscreenQuad();
	m_RenderToQuad->Initialise(windowWidth, windowHeight);

	m_HeatHaze = new HeatHaze();
	m_HeatHaze->Initialise(windowWidth, windowHeight);

	m_SceneLight.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_SceneLight.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_SceneLight.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_SceneLight.LightDirection = XMFLOAT3(5.0f, 5.0f, 0.0f);

	m_PreOffsetLightDir = m_SceneLight.LightDirection;

	m_SceneFog.FogStart = 5.0f;
	m_SceneFog.FogRange = 50.0f;
	m_SceneFog.FogColourDay = XMFLOAT4(0.7f, 0.80f, 0.92f, 1.0f);
	m_SceneFog.FogColourNight = XMFLOAT4(0.01f, 0.02f, 0.04f, 1.0f);
	m_SceneFog.FogColourSunriseSunset = XMFLOAT4(0.89f, 0.59f, 0.27f, 1.0f);
	m_SceneFog.UseFog = 1.0f;

	m_SpotLight.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_SpotLight.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_SpotLight.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_SpotLight.Attenuation = XMFLOAT3(0.4f, 0.02f, 0.0f);
	m_SpotLight.Spot = 20.0f;
	m_SpotLight.Range = 1000.0f;

	LoadMaterialsConfig();
}

void GraphicsHandler::Cleanup()
{
	m_HeatHaze->Cleanup();
	delete m_HeatHaze;
	m_HeatHaze = nullptr;

	m_SkyGradient->Cleanup();
	delete m_SkyGradient;
	m_SkyGradient = nullptr;

	m_Shadows->Cleanup();
	delete m_Shadows;
	m_Shadows = nullptr;

	m_BasicLight->Cleanup();
	delete m_BasicLight;
	m_BasicLight = nullptr;

	m_RenderToQuad->Cleanup();
	delete m_RenderToQuad;
	m_RenderToQuad = nullptr;
}

void GraphicsHandler::Update(UpdateEvent& e)
{
	XMFLOAT3 lightDir = m_PreOffsetLightDir;

	// Rotate the light at a 45 degree angle
	XMFLOAT3 rotationAxis = XMFLOAT3(0.0f, -0.5f, 0.5f);
	XMVECTOR axis = XMLoadFloat3(&rotationAxis);
	XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, m_LightRotationAmount * static_cast<float>(e.ElapsedTime));

	XMVECTOR rotatedLight = XMVector3Rotate(XMLoadFloat3(&lightDir), rotationQuaternion);
	XMStoreFloat3(&lightDir, rotatedLight);
	m_PreOffsetLightDir = lightDir;

	m_CurrentTime += m_LightRotationAmount * static_cast<float>(e.ElapsedTime);
	if (m_CurrentTime >= m_FullRotationAmount)
	{
		// Day has passed
		m_CurrentDay++;
		if (m_CurrentDay > 16)
			m_CurrentDay = 1;

		m_CurrentTime = 0.0f;

		if (m_CurrentDay % 4 == 0)
		{
			// Season has passed
			m_CurrentSeason++;
			if (m_CurrentSeason > 3)
				m_CurrentSeason = 0;
		}
	}
	else if (m_CurrentTime < 0.0f)
	{
		m_CurrentDay--;
		if (m_CurrentDay < 1)
			m_CurrentDay = 16;
		m_CurrentTime = m_FullRotationAmount;

		if (m_CurrentDay % 4 == 0)
		{
			// Season has passed
			m_CurrentSeason--;
			if (m_CurrentSeason < 0)
				m_CurrentSeason = 3;
		}
	}

	// Raise or lower the height of the sun
	lightDir.y += m_SeasonsSunHeightOffset[m_CurrentSeason];

	std::string CurrentSeasonString(m_SeasonNames.find(m_CurrentSeason)->second);
	if (CurrentSeasonString == "Spring" || CurrentSeasonString == "Autumn")
	{
		m_SceneFog.FogStart = 13.0f;
		m_SceneFog.FogRange = 150.0f;
	}
	else if (CurrentSeasonString == "Summer")
	{
		m_SceneFog.FogStart = 24.0f;
		m_SceneFog.FogRange = 240.0f;
	}
	else if (CurrentSeasonString == "Winter")
	{
		m_SceneFog.FogStart = 7.0f;
		m_SceneFog.FogRange = 80.0f;
	}

	if (InputHandler::IsKeyDown(Keys::OemPeriod))
	{
		m_LightRotationAmount += (0.5f * static_cast<float>(e.ElapsedTime));

		if (m_LightRotationAmount > 2.0f)
			m_LightRotationAmount = 2.0f;
	}

	if (InputHandler::IsKeyDown(Keys::OemComma))
	{
		m_LightRotationAmount -= (0.5f * static_cast<float>(e.ElapsedTime));

		if (m_LightRotationAmount < -2.0f)
			m_LightRotationAmount = -2.0f;
	}

	if (InputHandler::IsKeyDown(Keys::D1))
	{
		// Switch to Spring
		m_CurrentSeason = 0;
		m_CurrentDay = 1;

	}
	if (InputHandler::IsKeyDown(Keys::D2))
	{
		// Switch to Summer
		m_CurrentSeason = 1;
		m_CurrentDay = 5;
	}
	if (InputHandler::IsKeyDown(Keys::D3))
	{
		// Switch to Autumn
		m_CurrentSeason = 2;
		m_CurrentDay = 9;
	}
	if (InputHandler::IsKeyDown(Keys::D4))
	{
		// Switch to Winter
		m_CurrentSeason = 3;
		m_CurrentDay = 13;
	}

	if (InputHandler::IsKeyDown(Keys::T))
	{
		m_LightRotationAmount = 0.0f;
	}

	m_SceneLight.LightDirection = lightDir;
	m_Shadows->UpdateLightDirection(m_SceneLight.LightDirection);

	Scene* currScene = DX11Engine::Get().GetSceneHandler()->GetCurrentScene();
	Player const* currPlayer = DX11Engine::Get().GetPlayer();

	m_Shadows->SetSceneCentre(currPlayer->GetPlayerPosition());
	m_Shadows->BuildShadowTransform();

	m_SkyGradient->SetSceneCentre(currPlayer->GetPlayerPosition());
	m_SkyGradient->Update(e.ElapsedTime);

	m_BasicLight->CalculateLightColour(m_SceneLight, m_Shadows->GetLightPosition().y, m_SceneFog);

	m_HeatHaze->Update(static_cast<float>(e.TotalTime / 2.0));

	if (InputHandler::IsKeyDown(Keys::LeftControl) && InputHandler::IsKeyUp(Keys::R))
	{
		m_BasicLight->SetWireframeMode(!m_BasicLight->GetWireframeState());
	}

	m_SpotLight.Position = currPlayer->GetPlayerPosition();
	m_SpotLight.Direction = currPlayer->GetPlayerLookDirection();
}

void GraphicsHandler::Draw()
{
	const Scene* currScene = DX11Engine::Get().GetSceneHandler()->GetCurrentScene();
	const Player* currPlayer = DX11Engine::Get().GetPlayer();

	XMFLOAT3 sunPos = m_Shadows->GetLightPosition();
	m_SceneFog.SunHeight = sunPos.y;

	m_BasicLight->SetAsCurrentRenderTarget();
	m_BasicLight->SetAsCurrentViewport();
	RenderClass::DisableRtvClearing();

	m_SkyGradient->Render(currPlayer->GetCamera(), sunPos);
	RenderClass::EnableZBuffer();
	m_Shadows->Render(currScene->GetAllSceneElements());

	m_BasicLight->Render(currPlayer->GetCamera(), m_SceneLight, m_PointLights, m_SpotLight, m_SceneFog, currScene->GetAllSceneElements(), *m_Shadows);

	m_RenderToQuad->SetAsCurrentVertexShader();

	if (m_SeasonNames.find(m_CurrentSeason)->second == "Summer" || m_SeasonNames.find(m_CurrentSeason)->second == "Winter")
	{
		m_HeatHaze->Render(m_BasicLight->GetRenderTargetSRV(), m_SeasonNames.find(m_CurrentSeason)->second);
		m_RenderToQuad->Render(m_HeatHaze->GetTexture());
	}
	else
	{
		m_RenderToQuad->Render(m_BasicLight->GetRenderTargetSRV());
	}
}

ObjectMaterial GraphicsHandler::GetMaterialByName(char const* name) const
{
	std::string matName(name);

	auto const it = std::find_if(m_AllMaterials.cbegin(), m_AllMaterials.cend(), [matName](Material const& material)
		{
			return material.MaterialName == matName;
		});

	if (it != m_AllMaterials.cend())
	{
		return (*it).ObjMaterial;
	}

	return ObjectMaterial();
}

void GraphicsHandler::LoadMaterialsConfig()
{
	auto configLoader = ApplicationNew::Get().GetConfigLoader();
	std::vector<MaterialInfo> const allMaterials = configLoader->GetAllMaterials();
	assert(!allMaterials.empty());

	for (const MaterialInfo& matInfo : allMaterials)
	{
		Material material(matInfo.MaterialName.c_str(), ObjectMaterial(matInfo.MaterialAmbient, matInfo.MaterialDiffuse, matInfo.MaterialSpecular));
		
		m_AllMaterials.emplace_back(std::move(material));
	}
}

//void GraphicsHandler::CreateDefaultMaterialsConfig(std::string const& inConfigFilename)
//{
//}
