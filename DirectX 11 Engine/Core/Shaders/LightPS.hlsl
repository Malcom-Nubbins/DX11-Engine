#include "Common.hlsli"

Texture2D texColour : register(t0);
Texture2D texShadow : register(t1);
Texture2D texBumpmap : register(t2);

SamplerState samLinear : register(s0);
SamplerComparisonState samShadow : register(s1);

cbuffer ObjectValuesBuffer : register(b0)
{
    Surface surface;
    DirectionalLight dirLight;
    PointLight pointLight[96];
    SpotLight spotLight;

    float useColourTex;
    float useBumpMap;
    float usePointLight;
    int numOfLights;

    float3 EyePos;
    float affectedByLight;
}

cbuffer FogValuesBuffer : register(b1)
{
    float FogStart;
    float FogRange;
    float sunHeight;
    float fogValPad;

    float4 FogColourDay;
    float4 FogColourSunriseSunset;
    float4 FogColourNight;
}

struct VertexOutput
{
    float4 PosH : SV_POSITION0;
    float3 PosW : POSITION1;
    float3 NormW : NORMAL0;
    float3 TangentW : TANGENT0;
    float3 BinormalW : BINORMAL0;
    float2 Tex : TEXCOORD0;

    float4 ShadowProj : TEXCOORD1;
};

float4 main(VertexOutput input) : SV_TARGET
{
    float3 ambient = float3(0.0f, 0.0f, 0.0f);
    float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    float3 specular = float3(0.0f, 0.0f, 0.0f);

    input.NormW = normalize(input.NormW);

    float3 toEye = EyePos - input.PosW;
    float distToEye = length(toEye);
    toEye = normalize(toEye);

    float4 textureColour = texColour.Sample(samLinear, input.Tex);
    float3 bumpMapSample = texBumpmap.Sample(samLinear, input.Tex).rgb;

    if(affectedByLight == 1.0f)
    {
        float shadow = 1.0f;
        shadow = ComputeShadows(samShadow, texShadow, input.ShadowProj);

        if(useBumpMap == 1.0f)
            input.NormW = CalculateBumpedNormal(bumpMapSample, input.NormW, input.TangentW, input.BinormalW);

        float3 A, D, S;
        ComputeDirectionalLight(surface, dirLight, input.NormW, toEye, A, D, S);
        specular += shadow * S;
        diffuse += shadow * D;
        ambient += A;

        if(usePointLight == 1.0f)
        {
            if(numOfLights < 96)
            {
                for (int i = 0; i < numOfLights; ++i)
                {
                    ComputePointLight(surface, pointLight[i], input.PosW, input.NormW, toEye, A, D, S);
                    specular += S;
                    diffuse += D;
                }
            }
            else
            {
                for (int i = 0; i < 96; ++i)
                {
                    ComputePointLight(surface, pointLight[i], input.PosW, input.NormW, toEye, A, D, S);
                    specular += S;
                    diffuse += D;
                }
            }
        }

        ComputeSpotLight(surface, spotLight, input.PosW, input.NormW, toEye, A, D, S);
        specular += S;
        diffuse += D;
    
        float4 finalColour;
        if (useColourTex == 1.0f)
        {
            finalColour.rgb = (textureColour.rgb * (ambient + diffuse) + specular);
        }
        else
        {
            finalColour.rgb = (ambient + diffuse) + specular;
        }

        finalColour.a = surface.Diffuse.a;

        float fogLerp = saturate((distToEye - FogStart) / FogRange);

        if (sunHeight >= 20.0f)
        {
            finalColour = lerp(finalColour, FogColourDay, fogLerp);
        }
        else if(sunHeight <= 20.0f && sunHeight > -10.0f)
        {
            float sunPosClamped = clamp(sunHeight, -20.0f, 20.0f);
            if (sunHeight <= 20.0f && sunHeight > 0.0f)
            {
                float4 blendedFogColour = lerp(FogColourSunriseSunset, FogColourDay, sunPosClamped / 20);
                finalColour = lerp(finalColour, blendedFogColour, fogLerp);
            }
            else if (sunHeight <= 0.0f && sunHeight > -10.0f)
            {
                float4 blendedFogColour = lerp(FogColourSunriseSunset, FogColourNight, -sunPosClamped / 10);
                finalColour = lerp(finalColour, blendedFogColour, fogLerp);
            }
        }
        else
        {
            finalColour = lerp(finalColour, FogColourNight, fogLerp);
        }

        return finalColour;
    }
    else
    {
        float4 finalColour = float4(1.0f, 1.0f, 1.0f, 1.0f);

        return finalColour;
    }
}