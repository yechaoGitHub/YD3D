#include "Common.hlsli"

struct DepthVertexOut
{
    float4 svPosition : SV_POSITION;
};

DepthVertexOut main(VertexIn vin)
{
    DepthVertexOut vout = (DepthVertexOut) 0.0f;
    vout.svPosition = mul(ViewProj, float4(vin.Position, 1.0f));
    return vout;
}