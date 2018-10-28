#include "Application.h"
#include "Handlers/InputHandler.h"

namespace
{
	Application* app = 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return app->HandleInput(hWnd, message, wParam, lParam);
}

Application::Application(Timer* timer) : _timer(timer)
{
	_d3dClass = nullptr;
	_renderClass = nullptr;
	_shaderClass = nullptr;
	_bufferClass = nullptr;

	_mainScene = nullptr;
	_testingScene = nullptr;
	_player = nullptr;

	_windowWidth = WINDOW_WIDTH;
	_windowHeight = WINDOW_HEIGHT;
	app = this;
}

Application::~Application()
{
}

void Application::Cleanup()
{
	if(_testingScene != nullptr)
	{
		_testingScene->Cleanup();
		delete _testingScene;
		_testingScene = nullptr;
	}

	if (_mainScene != nullptr)
	{
		_mainScene->Cleanup();
		delete _mainScene;
		_mainScene = nullptr;
	}

	if(_player != nullptr)
	{
		_player->Cleanup();
		delete _player;
		_player = nullptr;
	}

	delete _bufferClass;
	_bufferClass = nullptr;

	_shaderClass->Cleanup();
	delete _shaderClass;
	_shaderClass = nullptr;

	_renderClass->Cleanup();
	delete _renderClass;
	_renderClass = nullptr;

	_d3dClass->Cleanup();
	delete _d3dClass;
	_d3dClass = nullptr;

	_windowClass->Cleanup();
	delete _windowClass;
	_windowClass = nullptr;
}

HRESULT Application::InitialiseApplication(HINSTANCE hinst, int cmdShow)
{
	HRESULT hr;
	_d3dClass = new D3DClass();
	_windowClass = new WindowClass();

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = WndProc;
	hr = _windowClass->InitWindow(wcex ,hinst, cmdShow, _windowWidth, _windowHeight);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Error creating window", L"Error", MB_OK);
		return hr;	
	}

	HWND hwnd = _windowClass->GetHWND();
	hr = _d3dClass->InitialiseDirectX(hwnd, static_cast<float>(_windowWidth), static_cast<float>(_windowHeight));
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to initialise DirectX", L"Error", MB_OK);
		return hr;
	}

	_renderClass = new RenderClass(_d3dClass);
	_renderClass->Initialise();
	_shaderClass = new ShaderClass(_d3dClass);
	_bufferClass = new BufferClass(_d3dClass);

	hr = _shaderClass->CreateSamplerStates();
	if (FAILED(hr))
		return hr;

	_d3dClass->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*D3D11_DEPTH_STENCIL_DESC equalDSDesc;
	ZeroMemory(&equalDSDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	equalDSDesc.DepthEnable = true;
	equalDSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	equalDSDesc.DepthFunc = D3D11_COMPARISON_EQUAL;

	hr = _d3dClass->GetDevice()->CreateDepthStencilState(&equalDSDesc, &DSEqual);
	if (FAILED(hr))
		return hr;*/

	_player = new Player(_windowClass);
	_player->Initialise();

	_textureHandler = new TextureHandler(_d3dClass);
	_textureHandler->LoadAllTextures();

	/*_testingScene = new TestingScene(_d3dClass, _shaderClass, _renderClass, _bufferClass, 
		_windowClass, _textureHandler, _timer, _player);

	_testingScene->InitialiseScene(_windowWidth, _windowHeight);*/

	_mainScene = new MainScene(_d3dClass, _shaderClass, _renderClass, _bufferClass, 
		_windowClass, _textureHandler, _timer, _player);

	_mainScene->InitialiseScene(_windowWidth, _windowHeight);

	return S_OK;
}

LRESULT Application::HandleInput(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		if (_windowClass != nullptr)
		{
			_windowWidth = LOWORD(lParam);
			_windowHeight = HIWORD(lParam);
			if (wParam == SIZE_MINIMIZED)
			{
				_windowClass->SetWindowMinimised(true);
				_windowClass->SetWindowMaximised(false);
				_timer->Stop();
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				_windowClass->SetWindowMinimised(false);
				_windowClass->SetWindowMaximised(true);
				_timer->Start();
				Resize(_windowWidth, _windowHeight);
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (_windowClass->GetWindowMinimised())
				{
					_windowClass->SetWindowMinimised(false);
					Resize(_windowWidth, _windowHeight);
				}
				else if (_windowClass->GetWindowMaximised())
				{
					_windowClass->SetWindowMaximised(false);
					Resize(_windowWidth, _windowHeight);
				}
				else if (_windowClass->GetWindowResizing())
				{

				}
				else
				{
					Resize(_windowWidth, _windowHeight);
				}
			}
		}
		return true;

	case WM_ENTERSIZEMOVE:
		_windowClass->SetWindowResizing(true);
		_timer->Stop();
		return true;

	case WM_EXITSIZEMOVE:
		_windowClass->SetWindowResizing(false);
		_timer->Start();
		Resize(_windowWidth, _windowHeight);
		return true;

	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);

	}

	return DefWindowProc(hwnd, message, wParam, lParam);

	return false;
}

void Application::Resize(float newWidth, float newHeight)
{
	if (_mainScene != nullptr)
	{
		_mainScene->ResizeViews(newWidth, newHeight);
	}

	if (_testingScene != nullptr)
	{
		_testingScene->ResizeViews(newWidth, newHeight);
	}

	if(_player != nullptr)
		_player->ResetPlayerCamera(newWidth, newHeight);
}

void Application::Update(float deltaTime)
{
	_player->Update(deltaTime);

	if(_testingScene != nullptr)
	{
		_testingScene->Update(deltaTime);
	}

	if (_mainScene != nullptr)
	{
		_mainScene->Update(deltaTime);
	}

	if(InputHandler::IsKeyDown(Esc))
	{
		ShowCursor(true);
		PostQuitMessage(0);
	}
}

void Application::Draw()
{
	if (_testingScene != nullptr)
	{
		_testingScene->Draw();
	}

	if (_mainScene != nullptr)
	{
		_mainScene->Draw();
	}

	_d3dClass->GetSwapChain()->Present(1, 0);
}
