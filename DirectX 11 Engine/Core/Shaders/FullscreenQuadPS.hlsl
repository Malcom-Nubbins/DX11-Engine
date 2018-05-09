Texture2D quadTex : register(t0);
SamplerState linearSampler : register(s0);

struct Vertex_Output
{
    float4 PosH : SV_POSITION0;
    float3 PosW : POSITION1;
    float2 Tex : TEXCOORD0;
};

float4 main(Vertex_Output input) : SV_TARGET
{
    float4 quadTexture = quadTex.Sample(linearSampler, input.Tex);
	return quadTexture;
}