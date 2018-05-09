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
    float padding;
};

float4 main(VertexOutput input) : SV_TARGET
{
    if(heatwave == 1.0f)
    {
        float4 distortionSample;
        distortionSample = texDistortionMap.Sample(linearSampler, float2(input.Tex.x, input.Tex.y + time));
        distortionSample.x = ((distortionSample.x * 2.0f) - 1.0f) * 0.005f;
        distortionSample.y = ((distortionSample.y * 2.0f) - 1.0f) * 0.005f;
        return texToOffset.Sample(linearSampler, input.Tex + distortionSample.xy);
    }

    if(blizzard == 1.0f)
    {
        float wavetime = sin(time * 2) - 1.1;

        // Snow layers
        float4 snowForeground = texDistortionMap.Sample(linearSampler, float2(input.Tex.x - time * 2, input.Tex.y - time * 2)) * 0.5f;
        float4 snowBackground = texDistortionMap.Sample(linearSampler, float2(input.Tex.x + wavetime, input.Tex.y - time / 2) * 3) * 0.5f;

        // Texture to overlay with snow
        float4 mainTexture = texToOffset.Sample(linearSampler, input.Tex);

        // Add back layer of snow, then front layer
        float4 combined = lerp(mainTexture, snowBackground, snowBackground.w);
        combined = lerp(combined, snowForeground, snowForeground.w);

        return combined;
    }

    return texToOffset.Sample(linearSampler, input.Tex);
}