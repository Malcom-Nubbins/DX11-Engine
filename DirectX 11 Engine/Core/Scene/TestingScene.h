#pragma once
#include "Scene.h"
#include "../Loaders/Terrain Generation/FlatTerrain.h"
#include "../Graphics/Graphical Effects/SkyColourGradient.h"
#include "../Graphics/Graphical Effects/RenderToFullscreenQuad.h"
#include "../Graphics/Graphical Effects/Shadows.h"
#include "../Graphics/Graphical Effects/BasicLight.h"
#include "../Handlers/TextureHandler.h"

class TestingScene : public Scene
{
private:
	std::vector<SceneElement*> _sceneElements;
	FlatTerrain* _flatPlane;

	DirectionalLight _sceneLight;

	ID3D11Buffer* _planeVertexBuffer;
	ID3D11Buffer* _planeIndexBuffer;

	BasicLight * _basicLight;
	Shadows* _shadows;
	RenderToFullscreenQuad* _renderToQuad;
	SkyColourGradient* _skyGradient;

	XMFLOAT2 _lastMousePos;

	SpotLight _spotLight;

	float _buttonCooldown;
	float _currentCooldown;

public:
	TestingScene(D3DClass* d3dClass, ShaderClass* shaderClass, RenderClass* renderClass,
		BufferClass* bufferClass, WindowClass* windowClass, TextureHandler* textureHandler, 
		Timer* timer, Player* player);
	~TestingScene();

	void Cleanup() override;
	void ResizeViews(float windowWidth, float windowHeight) override;
	void InitialiseScene(float windowWidth, float windowHeight) override;
	void InitialiseSceneGraphics(float windowWidth, float windowHeight) override;
	void Update(float delta) override;
	void Draw() override;
};

