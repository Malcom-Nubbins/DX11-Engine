static const float Pi = 3.141592654f;
static const float Pi2 = 6.283185307f;
static const float Pi_2 = 1.570796327f;
static const float Pi_4 = 0.7853981635f;
static const float InvPi = 0.318309886f;
static const float InvPi2 = 0.159154943f;

static const float Roughness = 0.1f;

struct Surface
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
};

struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 LightDirection;
    float padding;
};

struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 Position;
    float Range;

    float3 Attenuation;
    float padding;
};

struct SpotLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 Position;
    float Range;

    float3 Direction;
    float Spot;

    float3 Attenuation;
    float padding;
};

float3 SchlickFresnel(float3 f0, float3 h, float3 l)
{
    return f0 + (1.0f - f0) * pow((1.0f - dot(l, h)), 5.0f);
}

float GGX(float nDotV, float a)
{
    float k = a / 2;
    return nDotV / (nDotV * (1.0f - k) + k);
}

float GSmith(float a, float nDotV, float nDotL)
{
    return GGX(nDotL, a * a) * GGX(nDotV, a * a);
}

float3 DirectDiffuseBRDF(float3 diffuseAlbedo, float nDotL)
{
    return (diffuseAlbedo * nDotL) / Pi;
}

float3 DirectSpecularBRDF(float4 specularAlbedo, float3 normal, float3 lightDir, float3 eyePos)
{
    float3 halfVec = normalize(eyePos + lightDir);

    float nDotH = saturate(dot(normal, halfVec));
    float nDotL = saturate(dot(normal, lightDir));
    float nDotV = max(dot(normal, eyePos), 0.0001f);

    float alpha2 = specularAlbedo.w * specularAlbedo.w;

    float D = alpha2 / (Pi * pow(nDotH * nDotH * (alpha2 - 1) + 1, 2.0f));

    float3 F = SchlickFresnel(specularAlbedo.rgb, halfVec, lightDir);

    float G = GSmith(specularAlbedo.w, nDotV, nDotL);

    return D * F * G;
}


void ComputeDirectionalLight(Surface surface, DirectionalLight dirLight,
                                float3 lightDir, float3 normal, float3 toEye,
                                out float3 ambient, out float3 diffuse, out float3 specular)
{
    ambient = float3(0.0f, 0.0f, 0.0f);
    diffuse = float3(0.0f, 0.0f, 0.0f);
    specular = float3(0.0f, 0.0f, 0.0f);
    ambient = float3(surface.Ambient.rgb) * float3(dirLight.Ambient.rgb);

    float3 normalisedLightDir = normalize(dirLight.LightDirection);

    float diffuseAmount = dot(normalisedLightDir , normal);

    if (diffuseAmount > 0.0f)
    {
        diffuse = DirectDiffuseBRDF(surface.Diffuse.rgb, diffuseAmount) * dirLight.Diffuse.rgb;
        //diffuse = diffuseAmount * surface.Diffuse * dirLight.Diffuse;
        specular = DirectSpecularBRDF(surface.Specular, normal, normalisedLightDir, toEye) * dirLight.Specular.rgb;
    }
}

void ComputePointLight(Surface surface, PointLight pointLight, float3 pos,
                                float3 normal, float3 toEye,
                                out float3 ambient, out float3 diffuse, out float3 specular)
{
    ambient = float3(0.0f, 0.0f, 0.0f);
    diffuse = float3(0.0f, 0.0f, 0.0f);
    specular = float3(0.0f, 0.0f, 0.0f);

    float3 lightVec = pointLight.Position - pos;

    float distance = length(lightVec);

    lightVec /= distance;

    ambient = float3(surface.Ambient.rgb) * float3(pointLight.Ambient.rgb);

    float diffuseAmount = dot(lightVec, normal);

    [flatten]
    if(diffuseAmount > 0.0f)
    {
        float3 reflection = reflect(-lightVec, normal);
        float specularAmount = pow(max(dot(reflection, toEye), 0.0f), surface.Specular.w);

        diffuse = diffuseAmount * (float3(surface.Diffuse.rgb) * float3(pointLight.Diffuse.rgb));
        specular = specularAmount * (float3(surface.Specular.rgb) * float3(pointLight.Specular.rgb));
    }

    float attenuation = 1.0f / dot(pointLight.Attenuation, float3(1.0f, distance, distance * distance));

    diffuse *= attenuation / (distance * pointLight.Range);
    specular *= attenuation / (distance * pointLight.Range);
}

void ComputeSpotLight(Surface surface, SpotLight spotLight, float3 pos,
                                float3 normal, float3 toEye,
                                out float3 ambient, out float3 diffuse, out float3 specular)
{
    ambient = float3(0.0f, 0.0f, 0.0f);
    diffuse = float3(0.0f, 0.0f, 0.0f);
    specular = float3(0.0f, 0.0f, 0.0f);

    float3 lightVec = spotLight.Position - pos;

    float distance = length(lightVec);

    if (distance > spotLight.Range)
        return;

    lightVec /= distance;

    ambient = float3(surface.Ambient.rgb) * float3(spotLight.Ambient.rgb);

    float diffuseAmount = dot(lightVec, normal);

    if (diffuseAmount > 0.0f)
    {
        diffuse = DirectDiffuseBRDF(surface.Diffuse.rgb, diffuseAmount) * spotLight.Diffuse.rgb;
        //diffuse = diffuseAmount * surface.Diffuse * dirLight.Diffuse;
        specular = DirectSpecularBRDF(surface.Specular, normal, lightVec, toEye) * spotLight.Specular.rgb;
    }

    float spot = pow(max(dot(-lightVec, spotLight.Direction), 0.0f), spotLight.Spot);
    float attentuation = spot / dot(spotLight.Attenuation, float3(1.0f, distance, distance * distance));

    ambient *= spot;
    diffuse *= attentuation;
    specular *= attentuation;
}

float3 CalculateBumpedNormal(float3 normalMapSample, float3 normalW, float3 tangentW, float3 binormalW)
{
    float3 normalT = (2.0f * normalMapSample) - 1.0f;
    float3 bumpedNormal = (normalT.x * tangentW) + (normalT.y * binormalW) + (normalT.z * normalW);
    bumpedNormal = normalize(bumpedNormal);

    return bumpedNormal;
}

float ComputeShadows(SamplerComparisonState shadowSampler, Texture2D shadowMap, float4 shadowPosH)
{
    shadowPosH.xyz /= shadowPosH.w;
    float depth = shadowPosH.z;

    const float dx = 1.0f / 4096;
    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx, dx), float2(0.0f, dx), float2(dx, dx)
    };

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(shadowSampler, shadowPosH.xy + offsets[i], depth).r;
    }

    return percentLit /= 9.0f;
}