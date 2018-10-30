#pragma once
#include "System Handlers/D3DClass.h"
#include "System Handlers/RenderClass.h"
#include "System Handlers/ShaderClass.h"
#include "System Handlers/BufferClass.h"
#include "System Handlers/TextureHandler.h"
#include "System Handlers/WindowClass.h"

class SystemHandlers
{
private:
	D3DClass* _d3dClass;
	RenderClass* _renderClass;
	ShaderClass* _shaderClass;
	BufferClass* _bufferClass;
	TextureHandler* _textureHandler;
	WindowClass* _windowClass;

public:
	SystemHandlers(D3DClass* d3dClass, RenderClass* renderClass, ShaderClass* shaderClass, BufferClass* bufferClass,
		WindowClass* windowClass, TextureHandler* textureHandler);
	~SystemHandlers();

	void Cleanup();

public:
	D3DClass* GetD3DClass() const { return _d3dClass; }
	RenderClass* GetRenderClass() const { return _renderClass; }
	ShaderClass* GetShaderClass() const { return _shaderClass; }
	BufferClass* GetBufferClass() const { return _bufferClass; }
	TextureHandler* GetTextureHandler() const { return _textureHandler; }
	WindowClass* GetWindowClass() const { return _windowClass; }
};
