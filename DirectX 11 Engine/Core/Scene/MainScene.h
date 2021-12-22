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

class MainScene //: Scene
{
private:
	ID3D11Buffer* _planeVertexBuffer;
	ID3D11Buffer* _planeIndexBuffer;

	DiamondSquareTerrain* _diamondSquareTerrain;

public:
	MainScene(Player& player);
	~MainScene();
	/*void Cleanup() override;

	void PreResizeViews() override;
	void ResizeViews(float windowWidth, float windowHeight) override;

	void InitialiseScene(float windowWidth, float windowHeight) override;

	void Update(UpdateEvent& e) override;
	void Draw() override;*/
};

