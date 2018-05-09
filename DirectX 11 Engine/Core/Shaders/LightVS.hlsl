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
    float3 Normal : NORMAL0;
    float2 Tex : TEXCOORD0;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
};

struct VertexOutput
{
    float4 PosH : SV_POSITION0;
    float3 PosW : POSITION1;
    float3 NormW : NORMAL0;
    float2 Tex : TEXCOORD0;
    float3 TangentW : TANGENT0;
    float3 BinormalW : BINORMAL0;
    //float3 TangentTS : TANGENT1;
    //float3 BinormalTS : BINORMAL1;

    float4 ShadowProj : TEXCOORD1;
};

VertexOutput main( VertexInput input )
{
    VertexOutput output = (VertexOutput) 0;
    output.PosW = mul(float4(input.Pos, 1), World);
    output.PosH = mul(float4(output.PosW, 1), View);
    output.PosH = mul(output.PosH, Projection);
    output.Tex = input.Tex;

    output.NormW = normalize(mul(float4(input.Normal, 0.0f), World).xyz);

    output.TangentW = mul(float4(input.Tangent, 1.0f), World);
    output.BinormalW = mul(float4(input.Binormal, 1.0f), World);

    output.ShadowProj = mul(float4(input.Pos.xyz, 1.0f), mul(World, ShadowTransform));

	return output;
}