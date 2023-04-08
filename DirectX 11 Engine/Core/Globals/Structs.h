#pragma once
#include "stdafx.h"
#include <vector>

using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
	XMFLOAT3 Tangent;
	XMFLOAT3 Binormal;

	bool operator<(const SimpleVertex other) const
	{
		return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0;
	};
};

struct TerrainVertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
	XMFLOAT3 Tangent;
	XMFLOAT3 Binormal;

	TerrainVertex() : Binormal(), Normal(), Position(), Tangent(), TexCoord() {}
	TerrainVertex(const XMFLOAT3& pos, const XMFLOAT3& norm, const XMFLOAT3& tangent, const XMFLOAT3& binormal, const XMFLOAT2& tex) : Position(pos), Normal(norm), TexCoord(tex), Tangent(tangent), Binormal(binormal) {}
	TerrainVertex(const float px, const float py, const float pz,
	              const float nx, const float ny, const float nz,
	              const float tangx, const float tangy, const float tangz,
				const float binx, const float biny, const float binz,
	              const float texu, const float texv) : Position(px, py, pz), Normal(nx, ny, nz), TexCoord(texu, texv), Tangent(tangx, tangy, tangz), Binormal(binx, biny, binz) {}
};

struct Subset
{
	Subset() : VertexBuffer(nullptr), IndexBuffer(nullptr), VertexBufferStride(0), VertexBufferOffset(0), Id(-1),
	           IndexCount(0)
	{
	}

	TVertexBufferPtr VertexBuffer;
	TIndexBufferPtr IndexBuffer;

	UINT VertexBufferStride;
	UINT VertexBufferOffset;
	UINT Id;

	UINT IndexCount;
};

struct TempVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
};

struct SimpleQuad
{
	XMFLOAT3 Pos;
	XMFLOAT2 TexCoord;
};

struct NewObjectMesh
{
	NewObjectMesh() 
		: VertexBuffer(nullptr)
		, NumOfSubsets(0) 
	{}

	void IncreaseRefs()
	{
		if (VertexBuffer)
		{
			VertexBuffer->AddRef();
		}

		for (const Subset& subset : Subsets)
		{
			if (subset.VertexBuffer)
			{
				subset.VertexBuffer->AddRef();
			}

			if (subset.IndexBuffer)
			{
				subset.IndexBuffer->AddRef();
			}
		}
	}

	TVertexBufferPtr VertexBuffer;
	UINT NumOfSubsets;
	std::vector<Subset> Subsets;
};

struct ObjectMesh
{
	ObjectMesh() : VertexBuffer(nullptr)
		, IndexBuffer(nullptr)
		, vertexBufferStride(0)
		, VertexBufferOffset(0)
	{}

	TVertexBufferPtr VertexBuffer;
	TIndexBufferPtr IndexBuffer;

	UINT vertexBufferStride;
	UINT VertexBufferOffset;
	size_t NumberOfIndices = 0;
};

struct SurfaceMaterial
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
};

struct DirectionalLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 LightDirection;
	float Padding;
};

struct PointLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	XMFLOAT3 Position;
	float Range;

	XMFLOAT3 Attenuation;
	float Padding;
};

struct SpotLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	XMFLOAT3 Position;
	float Range;

	XMFLOAT3 Direction;
	float Spot;

	XMFLOAT3 Attenuation;
	float Padding;
};

struct ObjectMaterial
{
	ObjectMaterial(XMFLOAT4 const inAmb, XMFLOAT4 const inDiff, XMFLOAT4 const inSpec)
		: Diffuse(inDiff), Ambient(inAmb), Specular(inSpec)
	{}

	ObjectMaterial()
		: Diffuse(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)), Ambient(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)), Specular(XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f))
	{}

	XMFLOAT4 Diffuse;
	XMFLOAT4 Ambient;
	XMFLOAT4 Specular;
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
	SurfaceMaterial Surface;
	DirectionalLight DirLight;
	PointLight PointLight[96];
	SpotLight SpotLight;

	float UseColourTex;
	float UseBumpMap;
	float UsePointLights;
	float NumOfLights;

	XMFLOAT3 EyePos;
	float AffectedByLight;

	float UseSpecularMap;
	XMFLOAT3 Padding;
};

struct GradientValuesBuffer
{
	XMFLOAT4 HorizonColourSunrise;
	XMFLOAT4 OverheadColourSunrise;

	XMFLOAT4 HorizonColourDay;
	XMFLOAT4 OverheadColourDay;

	XMFLOAT4 HorizonColourNight;
	XMFLOAT4 OverheadColourNight;

	XMFLOAT3 SunPos;
	float Pad;
};

struct FogValuesBuffer
{
	float FogStart;
	float FogRange;
	float SunHeight;
	float UseFog;

	XMFLOAT4 FogColourDay;
	XMFLOAT4 FogColourSunriseSunset;
	XMFLOAT4 FogColourNight;
};

struct HeatHazeValues
{
	float Time;
	float Heatwave;
	float Blizzard;
	unsigned int SampleCount;
};

struct FullscreenQuadValues
{
	unsigned int SampleCount;
	XMFLOAT3 Padding;
};

struct MaterialInfo final
{
	MaterialInfo(char const* inMaterialName, XMFLOAT4 const inMatAmbient, XMFLOAT4 const inMatDiffuse, XMFLOAT4 const inMatSpecular)
		: MaterialName(inMaterialName), MaterialAmbient(inMatAmbient), MaterialDiffuse(inMatDiffuse), MaterialSpecular(inMatSpecular)
	{}

	std::string MaterialName;
	XMFLOAT4 MaterialAmbient;
	XMFLOAT4 MaterialDiffuse;
	XMFLOAT4 MaterialSpecular;
};

struct ConfigInfo final 
{
	ConfigInfo() : ConfigName(""), ConfigFilename(L"")
	{}

	ConfigInfo(char const* inConfigName, std::wstring const& inConfigFilename)
		: ConfigName(inConfigName), ConfigFilename(inConfigFilename)
	{}

	std::string ConfigName;
	std::wstring ConfigFilename;
};

struct UIElementInfo final
{
	UIElementInfo(std::string const& inName,
		XMFLOAT2 const& inSize, XMFLOAT2 const& offset, UIAnchorPoint const inAnchor, UIOriginPoint const inOrigin,
		std::string const& inTexName, uint32 order)
		: ElementName(inName)
		, Size(inSize)
		, Offset(offset)
		, AnchorPoint(inAnchor)
		, OriginPoint(inOrigin)
		, TextureName(inTexName)
		, Order(order)
	{}

	XMFLOAT2 Size;
	XMFLOAT2 Offset;
	UIAnchorPoint AnchorPoint;
	UIOriginPoint OriginPoint;
	std::string TextureName;
	std::string ElementName;
	uint32 Order;
};