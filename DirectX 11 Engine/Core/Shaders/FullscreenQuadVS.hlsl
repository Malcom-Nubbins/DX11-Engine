struct Vertex_Input
{
    float3 PosL : POSITION0;
    float2 TexL : TEXCOORD0;
};

struct Vertex_Output
{
    float4 PosH : SV_POSITION0;
    float3 PosW : POSITION1;
    float2 Tex : TEXCOORD0;
};

Vertex_Output main( Vertex_Input input )
{
    Vertex_Output output;
    output.PosH = float4(input.PosL, 1.0f);
    output.PosW = input.PosL;
    output.Tex = input.TexL;
	return output;
}