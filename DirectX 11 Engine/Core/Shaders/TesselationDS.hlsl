Texture2D texDisplacement : register(t0);
SamplerState samLinear : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix LightView;
    matrix Projection;
    matrix LightProjection;
    matrix TextureProject;
    matrix ShadowTransform;
}

cbuffer TesselationBuffer : register(b1)
{
    float MaxTessDistance;
    float MinTessDistance;
    float MinTessFactor;
    float MaxTessFactor;
};

cbuffer CamLightBuffer : register(b2)
{
    float4 EyePos;
    float4 LightVector;
};

struct DomainOutput
{
	float4 PosH  : SV_POSITION0;
    float3 PosW : POSITION1;
    float3 NormW : NORMAL0;
    float3 TangentW : TANGENT0;
    float3 BinormalW : BINORMAL0;
    float2 Tex : TEXCOORD0;

    float4 ShadowProj : TEXCOORD1;

    float3 EyePosTS : POSITION2;
    float3 LightVecTS : POSITION3;
    float3 NormalTS : NORMAL1;
};

//// Output control point
struct HullOutput
{
    float3 PosW : WORLDPOS0;
    float3 NormW : NORMAL0;
    float3 TangentW : TANGENT0;
    float3 BinormalW : BINORMAL0;
    float2 Tex : TEXCOORD0;

    float4 ShadowProj : TEXCOORD1;

    float3 EyePosTS : WORLDPOS1;
    float3 LightVecTS : WORLDPOS2;
    float3 NormalTS : NORMAL1;

    float TessFactor : TESS;
};

// Output patch constant data.
struct PatchTess
{
    float EdgeTessFactor[3] : SV_TessFactor; // e.g. would be [4] for a quad domain
    float InsideTessFactor : SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

[domain("tri")]
DomainOutput main(
	PatchTess input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HullOutput, 3> tri)
{
	DomainOutput output;

    output.PosW = domain.x * tri[0].PosW + domain.y * tri[1].PosW + domain.z * tri[2].PosW;
    output.NormW = domain.x * tri[0].NormW + domain.y * tri[1].NormW + domain.z * tri[2].NormW;
    output.TangentW = domain.x * tri[0].TangentW + domain.y * tri[1].TangentW + domain.z * tri[2].TangentW;
    output.BinormalW = domain.x * tri[0].BinormalW + domain.y * tri[1].BinormalW + domain.z * tri[2].BinormalW;
    output.Tex = domain.x * tri[0].Tex + domain.y * tri[1].Tex + domain.z * tri[2].Tex;

    output.EyePosTS = domain.x * tri[0].EyePosTS + domain.y * tri[1].EyePosTS + domain.z * tri[2].EyePosTS;
    output.LightVecTS = domain.x * tri[0].LightVecTS + domain.y * tri[1].LightVecTS + domain.z * tri[2].LightVecTS;
    output.NormalTS = domain.x * tri[0].NormalTS + domain.y * tri[1].NormalTS + domain.z * tri[2].NormalTS;

    output.ShadowProj = domain.x * tri[0].ShadowProj + domain.y * tri[1].ShadowProj + domain.z * tri[2].ShadowProj;

    output.NormW = normalize(output.NormW);
    output.EyePosTS = normalize(output.EyePosTS);
    output.LightVecTS = normalize(output.LightVecTS);
    output.NormalTS = normalize(output.NormalTS);

    const float MipInterval = 20.0f;
    float mipLevel = clamp((distance(output.PosW, EyePos.xyz) - MipInterval) / MipInterval, 0.0f, 6.0f);

    float height = texDisplacement.SampleLevel(samLinear, output.Tex, mipLevel).r;

    output.PosW += (0.2f * (height)) * output.NormW;

    output.PosH = mul(mul(float4(output.PosW, 1.0f), View), Projection);

	return output;
}
