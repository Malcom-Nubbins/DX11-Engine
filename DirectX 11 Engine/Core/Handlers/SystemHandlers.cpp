#include "SystemHandlers.h"

SystemHandlers::SystemHandlers(D3DClass* d3dClass, RenderClass* renderClass, ShaderClass* shaderClass,
	BufferClass* bufferClass, WindowClass* windowClass, TextureHandler* textureHandler):
	_d3dClass(d3dClass), _renderClass(renderClass), _shaderClass(shaderClass), _bufferClass(bufferClass),
	_textureHandler(textureHandler), _windowClass(windowClass)
{
}

SystemHandlers::~SystemHandlers()
{
}

void SystemHandlers::Cleanup()
{
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

	/*_windowClass->Cleanup();
	delete _windowClass;
	_windowClass = nullptr;*/
}
