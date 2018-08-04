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
    float3 TangentW : TANGENT0;
    float3 BinormalW : BINORMAL0;
    float2 Tex : TEXCOORD0;

    float4 ShadowProj : TEXCOORD1;

    float3 EyePosTS : POSITION2;
    float3 LightVecTS : POSITION3;
    float3 NormalTS : NORMAL1;

    float TessFactor : TESS;
};

VertexOutput main( VertexInput input )
{
    VertexOutput output = (VertexOutput) 0;
    output.PosW = mul(float4(input.Pos, 1), World);
    output.PosH = mul(float4(output.PosW, 1), View);
    output.PosH = mul(output.PosH, Projection);
    output.Tex = input.Tex;

    output.NormW = normalize(mul(float4(input.Normal, 0.0f), World).xyz);

    output.TangentW = mul(float4(input.Tangent, 0.0f), World);
    output.BinormalW = mul(float4(input.Binormal, 0.0f), World);

    output.ShadowProj = mul(float4(input.Pos.xyz, 1.0f), mul(World, ShadowTransform));
    
    // Tesselation
    float dist = distance(output.PosW, EyePos.xyz);
    float tess = saturate((MinTessDistance - dist) / (MinTessDistance - MaxTessDistance));

    output.TessFactor = MinTessFactor + tess * (MaxTessFactor - MinTessFactor);

    output.BinormalW = cross(output.NormW, output.TangentW);
    output.BinormalW = mul(float4(output.BinormalW, 0.0f), World);

    float3x3 tangentToWorldSpace;
    tangentToWorldSpace[0] = normalize(output.TangentW);
    tangentToWorldSpace[1] = normalize(output.BinormalW);
    tangentToWorldSpace[2] = output.NormW;

    tangentToWorldSpace = transpose(tangentToWorldSpace);

    float3 E = EyePos.xyz - output.PosW.xyz;

    output.EyePosTS = mul(E, tangentToWorldSpace);
    output.LightVecTS = mul(LightVector.xyz, tangentToWorldSpace);
    output.NormalTS = mul(output.NormW, tangentToWorldSpace);
    
	return output;
}