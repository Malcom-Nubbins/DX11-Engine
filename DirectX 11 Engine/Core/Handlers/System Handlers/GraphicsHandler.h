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

private:
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

	float _buttonCooldown;
	float _currentCooldown;

	std::vector<PointLight> _pointLights;

	// Move spotlight to player
	SpotLight _spotLight;
};

