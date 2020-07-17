#include "DepthPassVs.hlsl"

float main(DepthVertexOut pin) : SV_Target
{
    return pin.svPosition.z;
}