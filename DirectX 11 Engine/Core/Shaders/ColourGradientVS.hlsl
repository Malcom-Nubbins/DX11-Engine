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

struct VertexInput
{
    float3 Pos : POSITION0;
};

struct VertexOutput
{
    float4 PosH : SV_POSITION0;
    float4 SkyDomePos : TEXCOORD0;
};

VertexOutput main( VertexInput input )
{
    VertexOutput output;
    
    float3 posVS = mul(input.Pos.xyz, (float3x3) View);
    output.PosH = mul(float4(posVS, 1.f), Projection);
    output.SkyDomePos.xyz = input.Pos;
    output.SkyDomePos.w = 1.0f;

	return output;
}