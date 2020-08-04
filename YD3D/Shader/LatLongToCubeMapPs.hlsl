SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

Texture2D LatLongTexture : register(t0);

const static float2 invAtan = float2(0.1591, 0.3183);
float2 SampleSphericalMap(float3 v)
{
    float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

struct VertexOut
{
    float4 svPosition : SV_POSITION;
    float3 Position : POSITION;
};

float4 main(VertexOut vout) : SV_Target
{
    float2 uv = SampleSphericalMap(normalize(vout.Position));
    float3 color = LatLongTexture.Sample(gsamLinearClamp, uv);
    return float4(color, 1.0);
}
