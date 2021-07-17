cbuffer ColourGradientValues : register(b0)
{
    float4 horizonColourSunrise;
    float4 overheadColourSunrise;

    float4 horizonColourDay;
    float4 overheadColourDay;

    float4 horizonColourNight;
    float4 overheadColourNight;

    float3 sunPos;
    float pad;
}

struct VertexInput
{
    float3 Pos : POSITION0;
};

struct VertexOutput
{
    float4 PosH : SV_POSITION0;
    float4 SkyDomePos : TEXCOORD0;
};

float4 main(VertexOutput input) : SV_TARGET
{
    float height;
    float4 finalColour = float4(0.0f, 0.0f, 0.0f, 1.0f);

    height = input.SkyDomePos.y;

    if (height < 0.0f)
    {
        height = 0.0f;
    }

    if (sunPos.y >= 20.0f)
    {
        finalColour = lerp(horizonColourDay, overheadColourDay, height);
    }
    else if (sunPos.y <= 20.0f && sunPos.y > -10.0f)
    {
        float sunPosClamped = clamp(sunPos.y, -20.0f, 20.0f);
        if (sunPos.y <= 20.0f && sunPos.y > 0.0f)
        {
            float4 blendedHorizon = lerp(horizonColourSunrise, horizonColourDay, sunPosClamped / 20);
            float4 blendedOverhead = lerp(overheadColourSunrise, overheadColourDay, sunPosClamped / 20);
            finalColour = lerp(blendedHorizon, blendedOverhead, height);
        }
        else if (sunPos.y <= 0.0f && sunPos.y > -10.0f)
        {
            float4 blendedHorizon = lerp(horizonColourSunrise, horizonColourNight, -sunPosClamped / 10);
            float4 blendedOverhead = lerp(overheadColourSunrise, overheadColourNight, -sunPosClamped / 10);
            finalColour = lerp(blendedHorizon, blendedOverhead, height);
        }
    }
    else
    {
        finalColour = lerp(horizonColourNight, overheadColourNight, height);
    }

    return finalColour;
}