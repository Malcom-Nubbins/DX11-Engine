#pragma once
#include <vector>
#include <map>
#include "../Globals/Structs.h"
#include "Scene Elements/SceneElement.h"
#include "../Loaders/ModelLoader.h"
#include "../Handlers/System Handlers/MathsHandler.h"
#include "../Loaders/Terrain Generation/DiamondSquareTerrain.h"
#include "Scene.h"
#include "Player.h"
#include "../Graphics/Graphical Effects/BasicLight.h"
#include "../Graphics/Graphical Effects/SkyColourGradient.h"
#include "../Graphics/Graphical Effects/RenderToFullscreenQuad.h"
#include "../Graphics/Graphical Effects/HeatHaze.h"

class MainScene : Scene
{
private:
	DirectionalLight _sceneLight;
	FogValuesBuffer _sceneFog;

	ID3D11Buffer* _planeVertexBuffer;
	ID3D11Buffer* _planeIndexBuffer;

	std::vector<SceneElement*> _sceneElements;

	BasicLight* _basicLight;
	Shadows* _shadows;
	SkyColourGradient* _skyGradient;
	RenderToFullscreenQuad* _renderToQuad;
	HeatHaze* _heatHaze;

	DiamondSquareTerrain* _diamondSquareTerrain;

	float _lightRotationAmount;
	XMFLOAT3 _preOffsetLightDir;

	float _fullRotationAmount;

	float _currentTime;
	int _currentDay;
	int _currentSeason;

	std::vector<int> _seasonsSunHeightOffset;
	std::map<int, std::string> _seasonNames;

	std::vector<PointLight> _pointLights;
	SpotLight _spotLight;

	float _buttonCooldown;
	float _currentCooldown;

public:
	MainScene(SystemHandlers* systemHandlers, Timer* timer, Player* player);
	~MainScene() override;
	void Cleanup() override;

	void ResizeViews(float windowWidth, float windowHeight) override;

	void InitialiseScene(float windowWidth, float windowHeight) override;
	void InitialiseSceneGraphics(float windowWidth, float windowHeight) override;

	void Update(float deltaTime) override;
	void Draw() override;
};

