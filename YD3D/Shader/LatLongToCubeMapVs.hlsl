
cbuffer viewInfo : register(b0)
{
    float4x4	ViewProMatrix[6];
	uint		vpMatrixIndex;
};

struct VertexIn
{
    float3 Position :   POSITION;
    float3 Normal   :   NORMAL;
    float3 Tangant  :   TANGENT;
    float4 Color    :   COLOR;
    float2 TexCoord :   TEXCOORD;
};

struct VertexOut
{
    float4 svPosition : SV_POSITION;
    float3 Position   : POSITION;
};

VertexOut main(VertexIn vin)
{
    VertexOut vout = (VertexOut)0.0f;
    vout.Position = vin.Position;
    vout.svPosition = mul(ViewProMatrix[vpMatrixIndex], float4(vin.Position, 1.0f));
    
    return vout;
}
