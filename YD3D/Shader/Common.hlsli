#pragma once

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

Texture2D ModelTextures[8] : register(t0);

cbuffer ModelProperty : register(b0)
{
	float4x4 Model;
    float3 ModelPos;
    float3 ModelRotation;
    float3 ModelScalar;
};

cbuffer SceneInfo : register(b1)
{
    float4x4    View;
    float4x4    Project;
    float4x4    ViewProj;
    float3      CameraPos;
	float3      CameraDirection;    
    uint        PointLightCount;
    uint        SpotLightCount;
    float3      AmbientLight;
    float3      DirectionalLightDir;
    float3      DirectionalLightStrength;
};

struct LightDataStruct
{
    float3  Color;
    float   Intensity;
    float3  Position;
    float   Radius;
    float3  Diretion;
};

static const uint MAX_LIGHT_NUM = 1024;

cbuffer cbLightData : register(b2)
{
    LightDataStruct     LightData[MAX_LIGHT_NUM];
};

struct VertexIn
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float3 Tangant  : TANGENT;
    float4 Color	: COLOR;
	float2 TexCoord : TEXCOORD;
};

struct VertexOut
{
    float4 svPosition			: SV_POSITION;
    float3 WorldPosition		: POSITION1;
    float3 Normal				: NORMAL;
    float3 Tangent              : TANGENT;
    float4 Color				: COLOR;
    float2 TexCoord				: TEXCOORD;
};

static const float PI = 3.14159265359;
static const uint MODEL_ALBEDO_INDEX = 0;
static const uint MODEL_METALLIC_INDEX = 1;
static const uint MODEL_NORMAL_INDEX = 2;
static const uint MODEL_ROUGHNESS_INDEX = 3;