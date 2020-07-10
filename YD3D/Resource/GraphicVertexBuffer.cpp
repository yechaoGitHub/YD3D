#include "GraphicVertexBuffer.h"

namespace YD3D
{
	GraphicVertexBuffer::GraphicVertexBuffer() :
		mBufferLength(0)
	{
	}

	GraphicVertexBuffer::~GraphicVertexBuffer()
	{
	}

	bool GraphicVertexBuffer::Create(ID3D12Device* device, uint32_t stride, uint64_t count)
	{
		bool b = GraphicResource::Create(
					device,
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(stride * count),
					D3D12_RESOURCE_STATE_COMMON,
					nullptr);

		if (b) 
		{
			mVertexBuffer.BufferLocation = GetGpuAddress();
			mVertexBuffer.SizeInBytes = stride * count;
			mVertexBuffer.StrideInBytes = stride;
			mBufferLength = stride * count;

			return true;
		}
		else 
		{
			return false;
		}
	}

	uint64_t GraphicVertexBuffer::BufferLength()
	{
		return mBufferLength;
	}

	void GraphicVertexBuffer::Release()
	{
		GraphicResource::Release();
		mBufferLength = 0;
		mVertexBuffer = {};
	}

	const D3D12_VERTEX_BUFFER_VIEW& GraphicVertexBuffer::VertexView()
	{
		return mVertexBuffer;
	}
};


