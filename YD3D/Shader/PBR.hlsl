#include "Common.hlsli"

static const float MIN_ROUGHNESS = 0.0000001f;
static const float F0_NON_METAL = 0.04f;

// GGX (Trowbridge-Reitz)
float SpecDistribution(float3 n, float3 h, float roughness)
{
    float NdotH = max(dot(n, h), 0.0f);
    float NdotH2 = NdotH * NdotH;
    float a = roughness * roughness;
    float a2 = max(a * a, MIN_ROUGHNESS);

	// ((n dot h)^2 * (a^2 - 1) + 1)
    float denomToSquare = NdotH2 * (a2 - 1) + 1;

    return a2 / (PI * denomToSquare * denomToSquare);
}

// Schlick-GGX 
float GeometricShadowing(float3 n, float3 v, float3 h, float roughness)
{
	// End result of remapping:
    float k = pow(roughness + 1, 2) / 8.0f;
    float NdotV = max(dot(n, v), 0.0f);

	// Final value
    return NdotV / (NdotV * (1 - k) + k);
}

float3 FresnelSchlick(float cosTheta, float3 f0)
{
    return f0 + (1.0 - f0) * pow(1.0 - cosTheta, 5.0);
}

float3 FresnelEpic(float3 v, float3 h, float3 f0)
{
    float VdotH = max(dot(v, h), 0.0f);
    return f0 + (1 - f0) * exp2((-5.55473 * VdotH - 6.98316) * VdotH);
}

float LightFalloff(float distance, float radius)
{
    float v1 = pow(distance / radius, 4.0f);
    float v2 = saturate(1 - v1);
    float v3 = pow(v2, 2.0f);
    float v4 = distance * distance + 1;
    
    return v3 / v4;
}

float3 LambertDiffuse(float3 kS, float3 albedo, float metalness)
{
    float3 kD = (float3(1.0f, 1.0f, 1.0f) - kS) * (1 - metalness);
    return (kD * albedo / PI);
}

float3 CookTorrance(float3 n, float3 l, float3 v, float roughness, float metalness, float3 f0, out float3 kS)
{
    float3 h = normalize(v + l);

    float D = SpecDistribution(n, h, roughness);
    float3 F = FresnelEpic(v, h, f0);
    float G = GeometricShadowing(n, v, h, roughness) * GeometricShadowing(n, l, h, roughness);
    kS = F;
	//return (D * F * G) / (4 * max(dot(n, v), dot(n, l)));
    float NdotV = max(dot(n, v), 0.0f);
    float NdotL = max(dot(n, l), 0.0f);
    return (D * F * G) / (4 * max(NdotV * NdotL, 0.01f));
}

float3 DirectPBR(float lightIntensity, float3 lightColor, float3 n, float3 v, float3 l, float roughness, float metalness, float3 albedo, float ao)
{
    float3 f0 = lerp(F0_NON_METAL.rrr, albedo.rgb, metalness);
    float3 kS = float3(0.f, 0.f, 0.f);
    float3 specBRDF = CookTorrance(n, l, v, roughness, metalness, f0, kS);
    float3 diffBRDF = LambertDiffuse(kS, albedo, metalness);

    float NdotL = max(dot(n, l), 0.0);

    return (diffBRDF + specBRDF) * NdotL * lightIntensity * lightColor * ao;
}

float3 CalculatePointLight(LightDataStruct light, float3 worldPos, float3 worldNor, float3 cameraPos, float roughness, float metalness, float3 albedo, float ao)
{
    float distance = length(light.Position - worldPos);
    float lightIntensity = LightFalloff(distance, light.Radius) * light.Intensity;
    float3 viewVec = normalize(cameraPos - worldPos);
    float3 lightVec = normalize(light.Position - worldPos);
    
    return DirectPBR(lightIntensity, light.Color, worldNor, viewVec, lightVec, roughness, metalness, albedo, ao);
}

float3 GammaCorrection(float3 color, float gamma)
{
    return pow(color, float3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma));
}

float3 ColorLinear(float3 color, float gamma)
{
    return pow(color, float3(gamma, gamma, gamma));
}

float3 ACESToneMapping(float3 color, float adapted_lum)
{
    const float A = 2.51f;
    const float B = 0.03f;
    const float C = 2.43f;
    const float D = 0.59f;
    const float E = 0.14f;

    color *= adapted_lum;
    return (color * (A * color + B)) / (color * (C * color + D) + E);
}