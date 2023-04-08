#pragma once
#include "../Globals/Structs.h"
#include "../Timer.h"
#include "Player.h"
#include "../Loaders/Terrain Generation/FlatTerrain.h"
#include "../Core/Loaders/XMLLoader/rapidxml.hpp"

struct SceneElementInfo
{
	SceneElementInfo(const std::string& InObjectName, const uint32 InObjectCount,
		const bool InIsPlane, const bool InIsTerrain, const uint32 InTerrainGenWidth, const uint32 InTerrainGenHeight, const uint32 InTerrainSize,
		const XMFLOAT3 InTransformPos, const XMFLOAT3 InTransformScale, const XMFLOAT3 InTransformRot,
		const std::string& InModelName, const std::string& InMaterialName, const std::string& InTexName, const std::string& InNormalMapName, const std::string& InDisplacementMapName, const std::string& InSpecularMapName)
		: ObjectName(InObjectName), ObjectCount(InObjectCount)
		, bIsFlatPlane(InIsPlane), bIsTerrain(InIsTerrain), TerrainGenWidth(InTerrainGenWidth), TerrainGenHeight(InTerrainGenHeight), TerrainGenSize(InTerrainSize)
		, TransformPosition(InTransformPos), TranformScale(InTransformScale), TransformRotation(InTransformRot)
		, ModelName(InModelName), MaterialName(InMaterialName), TextureName(InTexName), NormalMapName(InNormalMapName), DisplacementMapName(InDisplacementMapName), SpecularMapName(InSpecularMapName)
	{}

	std::string ObjectName;
	uint32 ObjectCount;

	bool bIsFlatPlane;
	bool bIsTerrain;
	uint32 TerrainGenWidth;
	uint32 TerrainGenHeight;
	uint32 TerrainGenSize;

	XMFLOAT3 TransformPosition;
	XMFLOAT3 TranformScale;
	XMFLOAT3 TransformRotation;

	std::string ModelName;
	std::string MaterialName;
	std::string TextureName;
	std::string NormalMapName;
	std::string DisplacementMapName;
	std::string SpecularMapName;
};

struct SceneInfo
{
	SceneInfo(const std::string& InSceneName) 
		: SceneName(InSceneName)
	{}

	std::string SceneName;
	std::vector<SceneElementInfo> ElementInfos;
};

class Scene
{
protected:
	SceneInfo m_SceneInfo;
	std::vector<SceneElement*> m_SceneElements;

	bool m_bIsUnloading = false;
public:
	Scene(const SceneInfo& SceneInfo);
	virtual ~Scene();
	void Cleanup();

	void PreResizeViews();
	void ResizeViews(float windowWidth, float windowHeight);

	void Unload();

	void InitialiseScene();

	void Update(UpdateEvent& e);
	void Draw();

public:
	const std::vector<SceneElement*>& GetAllSceneElements() const { return m_SceneElements; }
	//Player* GetPlayer() const { return m_Player; }
};
