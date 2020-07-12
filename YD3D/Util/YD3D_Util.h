#pragma once
#include "YD3D_Define.h"
#include <d3d12.h>
#include <DirectXMath.h>
#include <string>
#include <stdint.h>

namespace YD3D
{
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

    class DxException
    {
    public:
        DxException() = default;
        DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

        std::wstring ToString()const;

        HRESULT ErrorCode = S_OK;
        std::wstring FunctionName;
        std::wstring Filename;
        int LineNumber = -1;
    };

    inline std::wstring AnsiToWString(const std::string& str)
    {
        WCHAR buffer[512];
        MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
        return std::wstring(buffer);
    }

    inline uint32_t CalcConstantBufferByteSize(uint32_t byteSize)
    {
        // Constant buffers must be a multiple of the minimum hardware
        // allocation size (usually 256 bytes).  So round up to nearest
        // multiple of 256.  We do this by adding 255 and then masking off
        // the lower 2 bytes which store all bits < 256.
        // Example: Suppose byteSize = 300.
        // (300 + 255) & ~255
        // 555 & ~255
        // 0x022B & ~0x00ff
        // 0x022B & 0xff00
        // 0x0200
        // 512
        return (byteSize + 255) & ~255;
    }

    inline bool IsNullCpuDescriptorHandle(const D3D12_CPU_DESCRIPTOR_HANDLE &cpuHandle)
    {
        return cpuHandle.ptr != 0;
    }

    inline bool IsNullGpuDescriptorHandle(const D3D12_CPU_DESCRIPTOR_HANDLE& gpuHandle)
    {
        return gpuHandle.ptr != 0;
    }

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

};
