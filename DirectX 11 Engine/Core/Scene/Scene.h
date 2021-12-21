#pragma once
#include "../Globals/Structs.h"
#include "../Timer.h"
#include "Player.h"
#include "../Handlers/SystemHandlers.h"
#include "../Loaders/Terrain Generation/FlatTerrain.h"

class Scene
{
protected:
	XMFLOAT2 _lastMousePos;
	
	Player* m_Player;
	std::vector<SceneElement*> _sceneElements;

	FlatTerrain* _flatPlane;

	VertexBuffer _planeVertexBuffer;
	IndexBuffer _planeIndexBuffer;

public:
	Scene(Player& player);
	virtual ~Scene();
	void Cleanup();

	void PreResizeViews();
	void ResizeViews(float windowWidth, float windowHeight);

	void InitialiseScene(float windowWidth, float windowHeight);
	//virtual void InitialiseSceneGraphics(float windowWidth, float windowHeight) = 0;

	void Update(UpdateEvent& e);
	void Draw();

public:
	std::vector<SceneElement*> GetAllSceneElements() const { return _sceneElements; }
	Player* GetPlayer() const { return m_Player; }
};
