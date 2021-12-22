#pragma once
#include "../Core/Graphics/Graphical Effects/BasicLight.h"
#include "../Core/Graphics/Graphical Effects/SkyColourGradient.h"
#include "../Core/Graphics/Graphical Effects/RenderToFullscreenQuad.h"
#include "../Core/Graphics/Graphical Effects/HeatHaze.h"

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
	struct S_Material final
	{
		S_Material(char const* inName, ObjectMaterial const material)
			: m_MaterialName(inName), m_Material(material)
		{}

		std::string m_MaterialName;
		ObjectMaterial m_Material;
	};

	void LoadMaterialsConfig();

	DirectionalLight _sceneLight;
	FogValuesBuffer _sceneFog;

	BasicLight* _basicLight;
	Shadows* _shadows;
	SkyColourGradient* _skyGradient;
	RenderToFullscreenQuad* _renderToQuad;
	HeatHaze* _heatHaze;

	float _lightRotationAmount;
	XMFLOAT3 _preOffsetLightDir;

	float _fullRotationAmount;

	float _currentTime;
	int _currentDay;
	int _currentSeason;

	std::vector<int> _seasonsSunHeightOffset;
	std::map<int, std::string> _seasonNames;

	std::vector<PointLight> _pointLights;

	std::vector<S_Material> m_AllMaterials;

	// Move spotlight to player
	SpotLight _spotLight;
};

