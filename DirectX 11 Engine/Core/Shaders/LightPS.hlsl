#include "Common.hlsli"

Texture2D texColour : register(t0);
Texture2D texShadow : register(t1);
Texture2D texBumpmap : register(t2);
Texture2D texSpecularMap : register(t3);

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

    float useSpecularMap;
    float3 padding;
}

cbuffer FogValuesBuffer : register(b1)
{
    float FogStart;
    float FogRange;
    float sunHeight;
    float UseFog;

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

    float3 EyePosTS : POSITION2;
    float3 LightVecTS : POSITION3;
    float3 NormalTS : NORMAL1;
};

float4 main(VertexOutput input) : SV_TARGET
{
    float3 ambient = float3(0.0f, 0.0f, 0.0f);
    float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    float3 specular = float3(0.0f, 0.0f, 0.0f);

    input.NormW = normalize(input.NormW);
    input.TangentW = normalize(input.TangentW);
    input.BinormalW = normalize(input.BinormalW);
    input.LightVecTS = normalize(input.LightVecTS);
    input.NormalTS = normalize(input.NormalTS);

    float3 toEye = EyePos - input.PosW;
    float3 toEyeTS = input.EyePosTS;
    float distToEye = length(toEye);

    toEyeTS = normalize(toEyeTS);
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
        ComputeDirectionalLight(surface, dirLight, input.LightVecTS, input.NormW, toEye, A, D, S);
        specular += shadow * S;
        diffuse += shadow * D;
        ambient += A;

        if(usePointLight == 1.0f)
        {
            if(numOfLights < 96)
            {
                for (int i = 0; i < numOfLights; ++i)
                {
                    ComputePointLight(surface, pointLight[i], input.PosW, input.NormalTS, toEyeTS, A, D, S);
                    specular += S;
                    diffuse += D;
                }
            }
            else
            {
                for (int i = 0; i < 96; ++i)
                {
                    ComputePointLight(surface, pointLight[i], input.PosW, input.NormalTS, toEyeTS, A, D, S);
                    specular += S;
                    diffuse += D;
                }
            }
        }


        ComputeSpotLight(surface, spotLight, input.PosW, input.NormW, toEye, A, D, S);
        specular += S;
        diffuse += D;

        if(useSpecularMap == 1.0f)
        {
            float specularIntensity = texSpecularMap.Sample(samLinear, input.Tex);
            specular *= specularIntensity;
        }

        float4 finalColour;

        if (useColourTex == 1.0f)
        {
            finalColour.rgb = textureColour.rgb * ((ambient + diffuse) + specular);
        }
        else
        {
            finalColour.rgb = (ambient + diffuse) + specular;
        }

        finalColour.a = surface.Diffuse.a;

        if(UseFog == 1.0f)
        {
            float fogLerp = saturate((distToEye - FogStart) / FogRange);

            if (sunHeight >= 20.0f)
            {
                finalColour = lerp(finalColour, FogColourDay, fogLerp);
            }
            else if (sunHeight <= 20.0f && sunHeight > -10.0f)
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
        }

        return finalColour;
    }
    else
    {
        float4 finalColour = dirLight.Diffuse * 2;

        return finalColour;
    }
}