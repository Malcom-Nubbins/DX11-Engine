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

void ComputeDirectionalLight(Surface surface, DirectionalLight dirLight,
                                float3 normal, float3 toEye,
                                out float3 ambient, out float3 diffuse, out float3 specular)
{
    ambient = float3(0.0f, 0.0f, 0.0f);
    diffuse = float3(0.0f, 0.0f, 0.0f);
    specular = float3(0.0f, 0.0f, 0.0f);

    ambient = surface.Ambient * dirLight.Ambient;

    float3 lightVec = normalize(dirLight.LightDirection);

    float diffuseAmount = dot(lightVec, normal);
    float specularAmount = 0.0f;

    if (diffuseAmount > 0.0f)
    {
        float3 reflection = reflect(-lightVec, normal);
        specularAmount = pow(max(dot(reflection, toEye), 0.0f), surface.Specular.w);

        diffuse = diffuseAmount * surface.Diffuse * dirLight.Diffuse;
        specular = specularAmount * surface.Specular * dirLight.Specular;
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

    if(distance > pointLight.Range)
        return;

    lightVec /= distance;

    ambient = surface.Ambient * pointLight.Ambient;

    float diffuseAmount = dot(lightVec, normal);

    [flatten]
    if(diffuseAmount > 0.0f)
    {
        float3 reflection = reflect(-lightVec, normal);
        float specularAmount = pow(max(dot(reflection, toEye), 0.0f), surface.Specular.w);

        diffuse = diffuseAmount * surface.Diffuse * pointLight.Diffuse;
        specular = specularAmount * surface.Specular * pointLight.Specular;
    }

    float attenuation = 1.0f / dot(pointLight.Attenuation, float3(1.0f, distance, distance * distance));

    diffuse *= attenuation;
    specular *= attenuation;
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

    ambient = surface.Ambient * spotLight.Ambient;

    float diffuseAmount = dot(lightVec, normal);

    [flatten]
    if (diffuseAmount > 0.0f)
    {
        float3 reflection = reflect(-lightVec, normal);
        float specularAmount = pow(max(dot(reflection, toEye), 0.0f), surface.Specular.w);

        diffuse = diffuseAmount * surface.Diffuse * spotLight.Diffuse;
        specular = specularAmount * surface.Specular * spotLight.Specular;
    }

    float spot = pow(max(dot(-lightVec, spotLight.Direction), 0.0f), spotLight.Spot);
    float attentuation = spot / dot(spotLight.Attenuation, float3(1.0f, distance, distance * distance));

    ambient *= spot;
    diffuse *= attentuation;
    specular *= attentuation;
}

float3 CalculateBumpedNormal(float3 normalMapSample, float3 normalW, float3 tangentW, float3 binormalW)
{
    //float3 normalT = (2.0f * normalMapSample) - 1.0f;
    //float3 bumpedNormal = (normalMapSample.x * tangentW) + (normalMapSample.y * binormalW) + (normalMapSample.z * normalW);
    //bumpedNormal = normalize(bumpedNormal);

    //return bumpedNormal;

    float3 normalT = (2.0f * normalMapSample) - 1.0f;

    float3 N = normalW;
    float3 T = normalize(tangentW - dot(tangentW, N) * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);
    float3 bumpedNormal = mul(normalT, TBN);

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