#include "Common.hlsl"

float4 PS(VertexOut Vout) : SV_Target
{
    //float3 Color = normalize(Vout.WorldPosition.zyz);    
    return float4(1, 1, 0, 1.0);
}