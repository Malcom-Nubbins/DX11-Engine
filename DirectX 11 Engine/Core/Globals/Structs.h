#pragma once
#include "stdafx.h"
#include <vector>

using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 texCoord;
	XMFLOAT3 tangent;
	XMFLOAT3 binormal;

	bool operator<(const SimpleVertex other) const
	{
		return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0;
	};
};

struct TerrainVertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 texCoord;
	XMFLOAT3 tangent;
	XMFLOAT3 binormal;

	TerrainVertex() : binormal(), normal(), position(), tangent(), texCoord() {}
	TerrainVertex(const XMFLOAT3& pos, const XMFLOAT3& norm, const XMFLOAT3& tangent, const XMFLOAT3& binormal, const XMFLOAT2& tex) : position(pos), normal(norm), texCoord(tex), tangent(tangent), binormal(binormal) {}
	TerrainVertex(const float px, const float py, const float pz,
	              const float nx, const float ny, const float nz,
	              const float tangx, const float tangy, const float tangz,
				const float binx, const float biny, const float binz,
	              const float texu, const float texv) : position(px, py, pz), normal(nx, ny, nz), texCoord(texu, texv), tangent(tangx, tangy, tangz), binormal(binx, biny, binz) {}
};

struct Subset
{
	Subset() : vertexBuffer(nullptr), indexBuffer(nullptr), vertexBufferStride(0), vertexBufferOffset(0), id(-1),
	           indexCount(0)
	{
	}

	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;

	UINT vertexBufferStride;
	UINT vertexBufferOffset;
	UINT id;

	UINT indexCount;
};

struct TempVertex
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 tex;
};

struct SimpleQuad
{
	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
};

struct NewObjectMesh
{
	NewObjectMesh() 
		: vertexBuffer(nullptr)
		, numOfSubsets(0) 
	{}

	void IncreaseRefs()
	{
		if (vertexBuffer)
		{
			vertexBuffer->AddRef();
		}

		for (auto const subset : subsets)
		{
			if (subset.vertexBuffer)
			{
				subset.vertexBuffer->AddRef();
			}

			if (subset.indexBuffer)
			{
				subset.indexBuffer->AddRef();
			}
		}
	}

	VertexBuffer vertexBuffer;
	UINT numOfSubsets;
	std::vector<Subset> subsets;
};

struct ObjectMesh
{
	ObjectMesh() : vertexBuffer(nullptr)
		, indexBuffer(nullptr)
		, vertexBufferStride(0)
		, vertexBufferOffset(0)
	{}

	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;

	UINT vertexBufferStride;
	UINT vertexBufferOffset;
	size_t numberOfIndices = 0;
};

struct SurfaceMaterial
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
};

struct DirectionalLight
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMFLOAT3 lightDirection;
	float padding;
};

struct PointLight
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;

	XMFLOAT3 position;
	float range;

	XMFLOAT3 attenuation;
	float padding;
};

struct SpotLight
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;

	XMFLOAT3 position;
	float range;

	XMFLOAT3 direction;
	float spot;

	XMFLOAT3 attenuation;
	float padding;
};

struct ObjectMaterial
{
	ObjectMaterial(XMFLOAT4 const inAmb, XMFLOAT4 const inDiff, XMFLOAT4 const inSpec)
		: diffuse(inDiff), ambient(inAmb), specular(inSpec)
	{}

	ObjectMaterial()
		: diffuse(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)), ambient(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)), specular(XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f))
	{}

	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;
	XMFLOAT4 specular;
};

struct MatrixBuffer
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX LightView;
	XMMATRIX Projection;
	XMMATRIX LightProjection;
	XMMATRIX TextureProject;
	XMMATRIX ShadowTransform;
};

struct ShadowDepthMatrixBuffer
{
	XMMATRIX World;
	XMMATRIX LightView;
	XMMATRIX LightProjection;
};

struct TesselationBuffer
{
	float MaxTessDistance;
	float MinTessDistance;
	float MinTessFactor;
	float MaxTessFactor;
};

struct CamLightBuffer
{
	XMFLOAT4 EyePos;
	XMFLOAT4 LightVector;
};

struct ObjectValuesBuffer
{
	SurfaceMaterial surface;
	DirectionalLight dirLight;
	PointLight pointLight[96];
	SpotLight spotLight;

	float useColourTex;
	float useBumpMap;
	float usePointLights;
	float numOfLights;

	XMFLOAT3 EyePos;
	float affectedByLight;

	float useSpecularMap;
	XMFLOAT3 padding;
};

struct GradientValuesBuffer
{
	XMFLOAT4 horizonColourSunrise;
	XMFLOAT4 overheadColourSunrise;

	XMFLOAT4 horizonColourDay;
	XMFLOAT4 overheadColourDay;

	XMFLOAT4 horizonColourNight;
	XMFLOAT4 overheadColourNight;

	XMFLOAT3 sunPos;
	float pad;
};

struct FogValuesBuffer
{
	float FogStart;
	float FogRange;
	float sunHeight;
	float UseFog;

	XMFLOAT4 FogColourDay;
	XMFLOAT4 FogColourSunriseSunset;
	XMFLOAT4 FogColourNight;
};

struct HeatHazeValues
{
	float time;
	float heatwave;
	float blizzard;
	float padding;;
};

struct FullscreenQuadValues
{
	unsigned int sampleCount;
	XMFLOAT3 padding;
};

struct S_TextureInfo final
{
	S_TextureInfo(char const* inTextureName, std::wstring const& inTexFilename)
		: m_TextureName(inTextureName)
		, m_TextureFilename(inTexFilename)
	{}
	std::string m_TextureName;
	std::wstring m_TextureFilename;
};

struct S_MaterialInfo final
{
	S_MaterialInfo(char const* inMaterialName, XMFLOAT4 const inMatAmbient, XMFLOAT4 const inMatDiffuse, XMFLOAT4 const inMatSpecular)
		: m_MaterialName(inMaterialName), m_MaterialAmbient(inMatAmbient), m_MaterialDiffuse(inMatDiffuse), m_MaterialSpecular(inMatSpecular)
	{}

	std::string m_MaterialName;
	XMFLOAT4 m_MaterialAmbient;
	XMFLOAT4 m_MaterialDiffuse;
	XMFLOAT4 m_MaterialSpecular;
};

struct S_ConfigInfo final 
{
	S_ConfigInfo() : m_ConfigName(""), m_ConfigFilename(L"")
	{}

	S_ConfigInfo(char const* inConfigName, std::wstring const& inConfigFilename)
		: m_ConfigName(inConfigName), m_ConfigFilename(inConfigFilename)
	{}

	std::string m_ConfigName;
	std::wstring m_ConfigFilename;
};

struct S_UIElementInfo final
{
	S_UIElementInfo(std::string const& inName,
		XMFLOAT2 const& inSize, XMFLOAT2 const& offset, UIAnchorPoint const inAnchor, UIOriginPoint const inOrigin,
		std::string const& inTexName, u32 order)
		: m_ElementName(inName)
		, m_Size(inSize)
		, m_Offset(offset)
		, m_AnchorPoint(inAnchor)
		, m_OriginPoint(inOrigin)
		, m_TextureName(inTexName)
		, m_Order(order)
	{}

	XMFLOAT2 m_Size;
	XMFLOAT2 m_Offset;
	UIAnchorPoint m_AnchorPoint;
	UIOriginPoint m_OriginPoint;
	std::string m_TextureName;
	std::string m_ElementName;
	u32 m_Order;
};