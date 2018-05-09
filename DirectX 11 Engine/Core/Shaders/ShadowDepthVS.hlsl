cbuffer ShadowDepthMatrixBuffer : register(b0)
{
    matrix World;
    matrix LightView;
    matrix LightProjection;
};

struct VertInput
{
    float3 Pos : POSITION0;
    float3 Normal : NORMAL0;
    float2 Tex : TEXCOORD0;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
};

struct VertOutput
{
    float4 PosH : SV_POSITION0;
    float2 Tex : TEXCOORD0;
};

VertOutput main( VertInput input)
{
    VertOutput output;

    output.PosH = mul(mul(float4(input.Pos, 1.0f), mul(World, LightView)), LightProjection);
    output.Tex = input.Tex;

	return output;
}