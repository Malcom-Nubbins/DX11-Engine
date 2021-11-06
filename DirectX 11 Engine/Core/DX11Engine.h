#pragma once
#include "EngineBase.h"
#include "Handlers/System Handlers/WindowClass.h"
#include "Globals/stdafx.h"

#include "Scene/Player.h"
#include "Scene/MainScene.h"
#include "Scene/TestingScene.h"
#include "Graphics/UI/UserInterface.h"

#include "Handlers/System Handlers/RenderClass.h"
#include "Handlers/System Handlers/ShaderClass.h"
#include "Handlers/System Handlers/BufferClass.h"
#include "Handlers/System Handlers/TextureHandler.h"

class DX11Engine : public EngineBase
{
public: 
	using super = EngineBase;

	DX11Engine(const std::wstring& name, UINT width, UINT height, bool vsync = false);

	virtual bool LoadContent() override;
	virtual void UnloadContent() override;
	virtual void Cleanup() override;

	static DX11Engine& Get();

	TextureHandler const* GetTextureHandler() const { return m_TextureHandler; }
	UserInterface const* GetUI() const { return m_UI; }

protected:
	virtual void OnUpdate(UpdateEvent& e) override;
	virtual void OnRender(RenderEvent& e) override;

	virtual void OnKeyPressed(KeyEvent& e) override;
	virtual void OnKeyReleased(KeyEvent& e) override;
	virtual void OnMouseWheel(MouseWheelEvent& e) override;
	virtual void PreOnResize() override;
	virtual void OnResize(UINT width, UINT height) override;
	virtual void OnMouseMoved(MouseMotionEvent& e) override;
	virtual void OnMouseButtonDown(MouseButtonEvent& e) override;
	virtual void OnMouseButtonUp(MouseButtonEvent& e) override;

	virtual void OnConfigsReloaded() override;

private:
	bool m_ContentLoaded;

	RenderClass* m_RenderClass;
	ShaderClass* m_ShaderClass;
	BufferClass* m_BufferClass;
	TextureHandler* m_TextureHandler;

	MainScene* m_MainScene;
	TestingScene* m_TestingScene;

	Player* m_Player;

	UserInterface* m_UI;

	float m_ToggleCooldown;
};

