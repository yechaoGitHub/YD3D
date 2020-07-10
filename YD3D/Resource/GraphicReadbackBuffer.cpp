#include "GraphicReadbackBuffer.h"

namespace YD3D
{
	GraphicReadbackBuffer::GraphicReadbackBuffer()
	{
	}

	GraphicReadbackBuffer::~GraphicReadbackBuffer()
	{
	}

	bool GraphicReadbackBuffer::Create(ID3D12Device* device, uint64_t length)
	{
		return GraphicResource::Create(
			device,
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(length),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr);
	}
};


