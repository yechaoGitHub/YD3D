#pragma once
#include "YD3D_Define.h"
#include <d3d12.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include <string>
#include <array>
#include <stdint.h>

namespace YD3D
{
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

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

    inline bool IsNullCpuDescriptorHandle(const D3D12_CPU_DESCRIPTOR_HANDLE &cpuHandle)
    {
        return cpuHandle.ptr != 0;
    }

    inline bool IsNullGpuDescriptorHandle(const D3D12_CPU_DESCRIPTOR_HANDLE& gpuHandle)
    {
        return gpuHandle.ptr != 0;
    }

    inline const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetCommonInputLayout()
    {
        static std::vector<D3D12_INPUT_ELEMENT_DESC> CommonInputLayOut =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 52, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };

        return CommonInputLayOut;
    }
    
    inline std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers()
    {
        // Applications usually only need a handful of samplers.  So just define them all up front
        // and keep them available as part of the root signature.  

        const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
            0, // shaderRegister
            D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
            D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

        const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
            1, // shaderRegister
            D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

        const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
            2, // shaderRegister
            D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
            D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

        const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
            3, // shaderRegister
            D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

        const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
            4, // shaderRegister
            D3D12_FILTER_ANISOTROPIC, // filter
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
            0.0f,                             // mipLODBias
            8);                               // maxAnisotropy

        const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
            5, // shaderRegister
            D3D12_FILTER_ANISOTROPIC, // filter
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
            0.0f,                              // mipLODBias
            8);                                // maxAnisotropy

        return {
            pointWrap, pointClamp,
            linearWrap, linearClamp,
            anisotropicWrap, anisotropicClamp };
    }

    inline const std::array<Vertex, 4>& GetFullScreenVertex()
    {
        static std::array<Vertex, 4> FullScreenVertex =
        {
            Vertex(-1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0), 
            Vertex(+1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0),
            Vertex(-1.0, +1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0),
            Vertex(+1.0, +1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0),
        };

        return FullScreenVertex;
    }

    inline const std::array<uint32_t, 6>& GetFullScreenIndex() 
    {
        static std::array<uint32_t, 6> FullScreenIndex =
        {
            0, 3, 1,
            0, 2, 3,
        };

        return FullScreenIndex;
    }

    inline const D3D12_GRAPHICS_PIPELINE_STATE_DESC& GetDefaultPSODesc() 
    {
        static D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = 
        {
            nullptr,
            D3D12_SHADER_BYTECODE{nullptr, 0},
            D3D12_SHADER_BYTECODE{nullptr, 0},
            D3D12_SHADER_BYTECODE{nullptr, 0},
            D3D12_SHADER_BYTECODE{nullptr, 0},
            D3D12_SHADER_BYTECODE{nullptr, 0},
            D3D12_STREAM_OUTPUT_DESC{},
            CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            UINT_MAX,
            CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
            D3D12_INPUT_LAYOUT_DESC{ GetCommonInputLayout().data(), static_cast<UINT>(GetCommonInputLayout().size()) },
            D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            1,
            { DXGI_FORMAT_R8G8B8A8_UNORM },
            DXGI_FORMAT_D24_UNORM_S8_UINT,
            DXGI_SAMPLE_DESC { 1, 0 },
            0,
            D3D12_CACHED_PIPELINE_STATE{ nullptr, 0},
            D3D12_PIPELINE_STATE_FLAG_NONE
        };
      
        return desc;
    }

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT GetResourceCopyableFootPrint(ID3D12Resource *res, uint32_t subresourceIndex);

    Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
        const std::wstring& filename,
        const D3D_SHADER_MACRO* defines,
        const std::string& entrypoint,
        const std::string& target);

};
