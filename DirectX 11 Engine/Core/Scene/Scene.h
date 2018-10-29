#pragma once
#include "../Handlers/D3DClass.h"
#include "../Globals/Structs.h"
#include "../Timer.h"
#include "../Handlers/TextureHandler.h"
#include "../Handlers/WindowClass.h"
#include "../Handlers/RenderClass.h"
#include "../Handlers/ShaderClass.h"
#include "../Handlers/BufferClass.h"
#include "Player.h"

class Scene
{
protected:
	D3DClass * _d3dClass;
	ShaderClass* _shaderClass;
	RenderClass* _renderClass;
	BufferClass* _bufferClass;
	WindowClass* _windowClass;
	TextureHandler* _textureHandler;
	Player* _player;

	Timer* _timer;
	DirectionalLight _sceneLight;
	XMFLOAT2 _lastMousePos;

public:
	Scene(D3DClass* d3dClass, ShaderClass* shaderClass, RenderClass* renderClass, 
		BufferClass* bufferClass, WindowClass* windowClass, TextureHandler* textureHandler,
		Timer* timer, Player* player);
	virtual ~Scene();
	virtual void Cleanup();

	virtual void ResizeViews(float windowWidth, float windowHeight);

	virtual void InitialiseScene(float windowWidth, float windowHeight);
	virtual void InitialiseSceneGraphics(float windowWidth, float windowHeight);

	virtual void Update(float delta);
	virtual void Draw();
};
