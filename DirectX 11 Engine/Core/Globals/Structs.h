#pragma once
#include "stdafx.h"

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

	TerrainVertex() {}
	TerrainVertex(const XMFLOAT3& pos, const XMFLOAT3& norm, const XMFLOAT3& tangent, const XMFLOAT2& tex) : Position(pos), Normal(norm), TexCoord(tex), Tangent(tangent) {}
	TerrainVertex(float px, float py, float pz,
		float nx, float ny, float nz,
		float tangx, float tangy, float tangz,
		float texu, float texv) : Position(px, py, pz), Normal(nx, ny, nz), Tangent(tangx, tangy, tangz), TexCoord(texu, texv) {}
};

struct TempVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 Tex;
};

struct SimpleQuad
{
	XMFLOAT3 Pos;
	XMFLOAT2 TexCoord;
};

struct ObjectMesh
{
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int numberOfIndices;

	UINT vertexBufferStride;
	UINT vertexBufferOffset;
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
	float padding;
};

struct PointLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	XMFLOAT3 Position;
	float Range;

	XMFLOAT3 Attenuation;
	float padding;
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
	float padding;
};

struct ObjectMaterial
{
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
	float padding;

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