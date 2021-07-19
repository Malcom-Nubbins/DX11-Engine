Texture2DMS<float4> quadTex : register(t0);
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
    float4 returnCol = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (uint i = 0; i < sampleCount; ++i)
    {
        returnCol += quadTex.Load(input.PosH, i);
    }
    
    return returnCol / sampleCount;
}