#include "YD3D_Util.h"
#include <comdef.h>

namespace YD3D
{
    DxException::DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
        ErrorCode(hr),
        FunctionName(functionName),
        Filename(filename),
        LineNumber(lineNumber)
    {
    }

    std::wstring DxException::ToString()const
    {
        // Get the string description of the error code.
        _com_error err(ErrorCode);
        std::wstring msg = err.ErrorMessage();

        return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
    }

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT GetResourceCopyableFootPrint(ID3D12Resource* res, uint32_t subresourceIndex)
    {
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT ret = {};
        ID3D12Device* dev(nullptr);
        ThrowIfFailed(res->GetDevice(IID_PPV_ARGS(&dev)));
        dev->GetCopyableFootprints(&res->GetDesc(), subresourceIndex, 1, 0, &ret, nullptr, nullptr, nullptr);
        return ret;
    }

    Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
        const std::wstring& filename,
        const D3D_SHADER_MACRO* defines,
        const std::string& entrypoint,
        const std::string& target)
    {
        UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
        compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        HRESULT hr = S_OK;

        Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errors;
        hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

        if (errors != nullptr)
            OutputDebugStringA((char*)errors->GetBufferPointer());

        ThrowIfFailed(hr);

        return byteCode;
    }

};