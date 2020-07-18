#include "CommonVs.hlsl"

float4 main(VertexOut Vout) : SV_Target
{
    return ModelTextures[0].Sample(gsamLinearClamp, Vout.TexCoord);
}