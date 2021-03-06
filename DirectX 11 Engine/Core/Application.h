#pragma once
#include "Globals\stdafx.h"
//#include "Globals\AppValues.h"

#include "Handlers/System Handlers/WindowClass.h"
#include "Handlers/System Handlers/D3DClass.h"
#include "Handlers/System Handlers/RenderClass.h"
#include "Handlers/System Handlers/ShaderClass.h"
#include "Handlers/System Handlers/BufferClass.h"
#include "Handlers/System Handlers/InputHandler.h"
#include "Timer.h"

#include "Scene/MainScene.h"
#include "Scene/TestingScene.h"
#include "Scene/Player.h"
#include "Graphics/UI/UserInterface.h"
#include "Handlers/System Handlers/TextureHandler.h"
#include "Handlers/SystemHandlers.h"

class Application
{
public:
	Application(Timer& timer);
	~Application();
	void Cleanup();

	HRESULT InitialiseApplication(HINSTANCE hinst, int cmdShow);

	LRESULT HandleInput(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	void Resize(float newWidth, float newHeight);

	void Update(float deltaTime);
	void Draw();

private:

private:
	WindowClass * _windowClass;
	D3DClass* _d3dClass;
	RenderClass* _renderClass;
	ShaderClass* _shaderClass;
	BufferClass* _bufferClass;	
	TextureHandler* _textureHandler;

	SystemHandlers* _systemHandlers;

	Timer& _timer;

	MainScene* _mainScene;
	TestingScene* _testingScene;

	Player* _player;

	UserInterface* _ui;

	ID3D11InputLayout* _simpleQuadInputLayout;

	UINT _windowWidth;
	UINT _windowHeight;
};

