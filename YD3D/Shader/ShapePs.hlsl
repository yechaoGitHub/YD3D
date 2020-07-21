#include "PBR.hlsl"

float4 main(VertexOut Vout) : SV_Target
{
    float3 lightOut = float3(0, 0, 0);
  
    float3 albedo = ModelTextures[0].Sample(gsamLinearClamp, Vout.TexCoord);
    
    for (uint i = 0; i < PointLightCount; i++)
    {
        lightOut += CaculatePointLight(LightData[i], Vout.WorldPosition, Vout.Normal, CameraPos, 1, 0.5, albedo, 1);
    }
    
    return float4(lightOut, 1.0f);
}
