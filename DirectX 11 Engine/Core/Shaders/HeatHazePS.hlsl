Texture2DMS<float4> texToOffset : register(t0);
Texture2D texDistortionMap : register(t1);
SamplerState linearSampler : register(s0);

struct VertexOutput
{
    float4 PosH : SV_POSITION0;
    float3 PosW : POSITION1;
    float2 Tex : TEXCOORD0;
};

cbuffer HeatHazeValues : register(b0)
{
    float time;
    float heatwave;
    float blizzard;
    unsigned int sampleCount;
};

float4 main(VertexOutput input) : SV_TARGET
{
    if(heatwave == 1.0f)
	{
		float4 distortionSample;
		distortionSample = texDistortionMap.Sample(linearSampler, float2(input.Tex.x, input.Tex.y + time));
		distortionSample.x = ((distortionSample.x * 2.0f) - 1.0f) * 0.005f;
		distortionSample.y = ((distortionSample.y * 2.0f) - 1.0f) * 0.005f;
        
		float4 returnCol = float4(0.0f, 0.0f, 0.0f, 1.0f);
		for (uint i = 0; i < sampleCount; ++i)
		{
            returnCol += texToOffset.Load(int2(input.PosH.xy), i);
        }
        
		return returnCol / sampleCount;
	}
    
    if(blizzard == 1.0f)
    {
        float wavetime = sin(time) - 1.1f;

        // Snow layers
        float4 snowForeground = texDistortionMap.Sample(linearSampler, float2(input.Tex.x - time, input.Tex.y - time)) * 0.5f;
        float4 snowBackground = texDistortionMap.Sample(linearSampler, float2(input.Tex.x + wavetime, input.Tex.y - time / 2) * 3) * 0.5f;

		float4 returnCol = float4(0.0f, 0.0f, 0.0f, 1.0f);
		for (uint i = 0; i < sampleCount; ++i)
		{
            returnCol += texToOffset.Load(int2(input.PosH.xy), i);
        }
        
        // Texture to overlay with snow

        // Add back layer of snow, then front layer
		float4 combined = lerp((returnCol / sampleCount), snowBackground, snowBackground.w);
        combined = lerp(combined, snowForeground, snowForeground.w);

        return combined;
    }
    
	float4 returnCol = float4(0.0f, 0.0f, 0.0f, 1.0f);
	for (uint i = 0; i < sampleCount; ++i)
	{
		returnCol += texToOffset.Load(input.Tex, i);
	}

    return returnCol / sampleCount;
}