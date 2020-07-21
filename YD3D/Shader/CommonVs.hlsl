#include "Common.hlsli"

VertexOut main(VertexIn Vin)
{
    VertexOut Vout = (VertexOut) 0.0;
    
    float4 WorldPos = mul(Model, float4(Vin.Position, 1.0f));
    
    Vout.WorldPosition = WorldPos.xyz;
    Vout.Normal = mul(Model, float4(Vin.Normal, 0.0f));
    Vout.Tangent = mul(Model, float4(Vin.Tangant, 0.0f));
    Vout.Color = Vin.Color;
    Vout.TexCoord = Vin.TexCoord; //mul(Model, float4(Vin.TexCoord, 0, 1.0f));
    Vout.svPosition = mul(ViewProj, WorldPos);
    
    return Vout;
}
