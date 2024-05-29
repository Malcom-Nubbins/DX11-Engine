#pragma once
#include "../Core/Graphics/Graphical Effects/BasicLight.h"
#include "../Core/Graphics/Graphical Effects/SkyColourGradient.h"
#include "../Core/Graphics/Graphical Effects/RenderToFullscreenQuad.h"
#include "../Core/Graphics/Graphical Effects/HeatHaze.h"
#include "../Core/Globals/Events.h"

class GraphicsHandler
{
public:
	GraphicsHandler();
	~GraphicsHandler();

	void PreResizeViews();
	void ResizeViews(float const windowWidth, float const windowHeight);

	void Init(float const windowWidth, float const windowHeight);
	void Cleanup();

	void Update(UpdateEvent& e);
	void Draw();

	ObjectMaterial GetMaterialByName(char const* name) const;

private:
	struct Material final
	{
		Material(char const* inName, ObjectMaterial const material)
			: MaterialName(inName), ObjMaterial(material)
		{}

		std::string MaterialName;
		ObjectMaterial ObjMaterial;
	};

	void LoadMaterialsConfig();

	DirectionalLight m_SceneLight;
	FogValuesBuffer m_SceneFog;

	BasicLight* m_BasicLight;
	Shadows* m_Shadows;
	SkyColourGradient* m_SkyGradient;
	RenderToFullscreenQuad* m_RenderToQuad;
	HeatHaze* m_HeatHaze;

	float m_LightRotationAmount;
	XMFLOAT3 m_PreOffsetLightDir;

	float m_FullRotationAmount;

	float m_CurrentTime;
	int m_CurrentDay;
	int m_CurrentSeason;

	std::vector<float> m_SeasonsSunHeightOffset;
	std::map<int, std::string> m_SeasonNames;

	std::vector<PointLight> m_PointLights;

	std::vector<Material> m_AllMaterials;

	// Move spotlight to player
	SpotLight m_SpotLight;
};

