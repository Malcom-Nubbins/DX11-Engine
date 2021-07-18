Texture2DMS<float4> quadTex : register(t0);
SamplerState linearSampler : register(s0);

struct Vertex_Output
{
    float4 PosH : SV_POSITION0;
    float3 PosW : POSITION1;
    float2 Tex : TEXCOORD0;
};

float4 main(Vertex_Output input) : SV_TARGET
{
    float4 returnCol = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (uint i = 0; i < 4; ++i)
    {
        returnCol += quadTex.Load(input.PosH, i);
    }
    
    return returnCol / 4;
}