#pragma once
#include <vector>
#include <map>

#include "../Globals/Structs.h"
#include "Scene Elements\SceneElement.h"

#include "Graphical Effects\BasicLight.h"
#include "Graphical Effects\Shadows.h"
#include "Graphical Effects\SkyColourGradient.h"
#include "Graphical Effects\RenderToFullscreenQuad.h"
#include "Graphical Effects\HeatHaze.h"

#include "Graphical Effects\Textures\TextureHandler.h"
#include "../Loaders/OBJLoader.h"
#include "../Loaders/Terrain Generation/DiamondSquareTerrain.h"
#include "Scene.h"

class MainScene : Scene
{
private:
	DirectionalLight _sceneLight;
	FogValuesBuffer _sceneFog;

	ID3D11Buffer* _planeVertexBuffer;
	ID3D11Buffer* _planeIndexBuffer;

	ID3D11Buffer* _matrixBuffer;
	ID3D11Buffer* _objectValueBuffer;

	std::vector<SceneElement*> _sceneElements;

	BasicLight* _basicLight;
	Shadows* _shadows;
	SkyColourGradient* _skyGradient;
	RenderToFullscreenQuad* _renderToQuad;
	HeatHaze* _heatHaze;

	DiamondSquareTerrain* _diamondSquareTerrain;

	XMFLOAT2 _lastMousePos;

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
	MainScene(D3DClass* d3dClass, ShaderClass* shaderClass, RenderClass* renderClass, 
		BufferClass* bufferClass, WindowClass* windowClass, TextureHandler* textureHandler, 
		Timer* timer);
	~MainScene() override;
	void Cleanup() override;

	void ResizeViews(float windowWidth, float windowHeight) override;

	void InitialiseScene(float windowWidth, float windowHeight) override;
	void InitialiseSceneGraphics(float windowWidth, float windowHeight) override;

	void HandleMouse(WPARAM btnState, int x, int y) override;

	void Update(float deltaTime) override;
	void Draw() override;
};

