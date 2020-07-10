#include "GraphicRenderTarget.h"

namespace YD3D
{
	GraphicRenderTarget::GraphicRenderTarget()
	{
	}

	GraphicRenderTarget::~GraphicRenderTarget()
	{
	}

	bool GraphicRenderTarget::Create(ID3D12Device* device, UINT width, UINT height, UINT arraySize, UINT mipLevel, DXGI_FORMAT format)
	{
		return GraphicResource::Create(device,
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, arraySize, mipLevel, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr);
	}
};
