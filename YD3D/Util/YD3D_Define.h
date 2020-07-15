#pragma once

#include <functional>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>

namespace YD3D
{
#define ALIGN_16	__declspec(align(16))
#define ALIGN(v)	__declspec(align(v))

#define ANY_DESCRIPTOR_HEAP_POS -1 

    typedef std::function<void(D3D12_COMMAND_LIST_TYPE, uint64_t)>	GraphicTaskCallbackFunction, CommandQueueCallbackFunction;
    typedef std::function<bool(ID3D12GraphicsCommandList*)> GraphicTaskFunction;

    enum ECommandQueueType { ERENDER, ECOPY, ECOMPUTE, ESWAP_CHAIN };
    enum EShaderType { VS = 0, PS, DS, HS, GS };

    inline std::wstring _HLSL_FILE_PATH_ = L"Shader/";

    inline const float Infinity = FLT_MAX;
    inline const float Pi = 3.1415926535f;

    inline DirectX::XMFLOAT4X4 Identity4x4()
    {
        static DirectX::XMFLOAT4X4 I(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        return I;
    }

    ALIGN(0) struct Vertex
    {
        Vertex() {}
        Vertex(
            const DirectX::XMFLOAT3& p,
            const DirectX::XMFLOAT3& n,
            const DirectX::XMFLOAT3& t,
            const DirectX::XMFLOAT2& uv) :
            Position(p),
            Normal(n),
            Tangent(t),
            TexCrood(uv) {}

        Vertex(
            float px, float py, float pz,
            float nx, float ny, float nz,
            float tx, float ty, float tz,
            float u, float v) :
            Position(px, py, pz),
            Normal(nx, ny, nz),
            Tangent(tx, ty, tz),
            TexCrood(u, v) {}

        DirectX::XMFLOAT3 Position = {};
        DirectX::XMFLOAT3 Normal = {};
        DirectX::XMFLOAT3 Color = {};
        DirectX::XMFLOAT3 Tangent = {};
        DirectX::XMFLOAT2 TexCrood = {};
    };

    ALIGN_16 struct PointLight
    {
        DirectX::XMFLOAT3 CameraPos = {};
        DirectX::XMFLOAT3 CameraDirection = {};
    };
};

