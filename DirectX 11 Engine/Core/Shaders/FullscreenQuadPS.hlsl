Texture2D quadTex : register(t0);
SamplerState linearSampler : register(s0);

cbuffer ValuesBuffer : register(b0)
{
    unsigned int sampleCount;
    float3 padding;
}

struct Vertex_Output
{
    float4 PosH : SV_POSITION0;
    float3 PosW : POSITION1;
    float2 Tex : TEXCOORD0;
};

float4 main(Vertex_Output input) : SV_TARGET
{ 
    return quadTex.Sample(linearSampler, input.Tex);
}