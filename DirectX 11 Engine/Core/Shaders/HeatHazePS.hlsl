Texture2D texToOffset : register(t0);
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
        
        float4 returnCol = texToOffset.Sample(linearSampler, input.Tex + distortionSample.xy);
        
        return returnCol;
    }
    
    if(blizzard == 1.0f)
    {
        float wavetime = sin(time) - 1.1f;

        // Snow layers
        float4 snowForeground = texDistortionMap.Sample(linearSampler, float2(input.Tex.x - time, input.Tex.y - time)) * 0.5f;
        float4 snowBackground = texDistortionMap.Sample(linearSampler, float2(input.Tex.x + wavetime, input.Tex.y - time / 2) * 3) * 0.5f;

        float4 returnCol = texToOffset.Sample(linearSampler, input.Tex);
		
        // Texture to overlay with snow

        // Add back layer of snow, then front layer
		float4 combined = lerp(returnCol, snowBackground, snowBackground.w);
        combined = lerp(combined, snowForeground, snowForeground.w);

        return combined;
    }

    return texToOffset.Sample(linearSampler, input.Tex);
}