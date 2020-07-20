#include "CommonVs.hlsl"

float4 main(VertexOut Vout) : SV_Target
{
 
    float3 pos = LightData[1].Position;
    
    return ModelTextures[0].Sample(gsamLinearClamp, Vout.TexCoord);
}
