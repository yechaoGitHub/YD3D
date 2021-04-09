#include "PBR.hlsl"

float3 CalculateTBN(VertexOut vout, float3 bumpMapNor)
{
    float3 normal = normalize(vout.Normal);
    float3 tangent = normalize(vout.Tangent);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    float3 bitangent = cross(normal, tangent);
    bumpMapNor = 2.0 * bumpMapNor - float3(1.0, 1.0, 1.0);
    
    float3 newNormal;
    float3x3 tbn = float3x3(tangent, bitangent, normal);
    newNormal = mul(bumpMapNor, tbn);
    newNormal = normalize(newNormal);
    return newNormal;
}

float4 main(VertexOut Vout) : SV_Target
{
    float3 lightOut = float3(0, 0, 0);
    
    float3 albedo = ColorLinear(ModelTextures[MODEL_ALBEDO_INDEX].Sample(gsamLinearClamp, Vout.TexCoord), 2.2);
    float3 metallic = ModelTextures[MODEL_METALLIC_INDEX].Sample(gsamLinearClamp, Vout.TexCoord);
    float3 normal = ModelTextures[MODEL_NORMAL_INDEX].Sample(gsamLinearClamp, Vout.TexCoord);
    float3 roughness = ModelTextures[MODEL_ROUGHNESS_INDEX].Sample(gsamLinearClamp, Vout.TexCoord);
    
    for (uint i = 0; i < PointLightCount; i++)
    {
        lightOut += CalculatePointLight(LightData[i], Vout.WorldPosition, CalculateTBN(Vout, normal), CameraPos, roughness, metallic, albedo, 1);
    }
    
    return float4(GammaCorrection(lightOut, 2.2), 1.0f);
}
