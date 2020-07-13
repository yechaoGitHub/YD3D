#include "Common.hlsl"

VertexOut VS(VertexIn Vin)
{
    VertexOut Vout = (VertexOut) 0.0;
    float4x4 ViewProjectMat = mul(Project, View);
    float4 WorldPos = mul(Model, float4(Vin.Position, 1.0f));

    Vout.WorldPosition = WorldPos.xyz;
    Vout.svPosition = mul(ViewProjectMat, WorldPos);
   
    return Vout;
}