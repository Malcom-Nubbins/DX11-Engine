
// Input control point
struct VertexOutput
{
    float4 PosH : SV_POSITION0;
    float3 PosW : WORLDPOS0;
    float3 NormW : NORMAL0;
    float3 TangentW : TANGENT0;
    float3 BinormalW : BINORMAL0;
    float2 Tex : TEXCOORD0;

    float4 ShadowProj : TEXCOORD1;

    float TessFactor : TESS;
};

// Output control point
struct HullOutput
{
    float3 PosW : WORLDPOS0;
    float3 NormW : NORMAL0;
    float3 TangentW : TANGENT0;
    float3 BinormalW : BINORMAL0;
    float2 Tex : TEXCOORD0;

    float4 ShadowProj : TEXCOORD1;

    float TessFactor : TESS;
};

// Output patch constant data.
struct PatchTess
{
	float EdgeTessFactor[3]	: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor : SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

PatchTess PatchHS(InputPatch<VertexOutput, 3> patch, uint patchID : SV_PrimitiveID)
{
    PatchTess pt;
    pt.EdgeTessFactor[0] = 0.5f * (patch[1].TessFactor + patch[2].TessFactor);
    pt.EdgeTessFactor[1] = 0.5f * (patch[2].TessFactor + patch[0].TessFactor);
    pt.EdgeTessFactor[2] = 0.5f * (patch[0].TessFactor + patch[1].TessFactor);
    pt.InsideTessFactor = pt.EdgeTessFactor[0];

    return pt;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchHS")]
HullOutput main( 
	InputPatch<VertexOutput, 3> ip, 
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	HullOutput output;

	// Insert code to compute Output here
	output.PosW = ip[i].PosW;
    output.NormW = ip[i].NormW;
    output.TangentW = ip[i].TangentW;
    output.BinormalW = ip[i].BinormalW;
    output.Tex = ip[i].Tex;
    output.ShadowProj = ip[i].ShadowProj;
    output.TessFactor = ip[i].TessFactor;

	return output;
}
