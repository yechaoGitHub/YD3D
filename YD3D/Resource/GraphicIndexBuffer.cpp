#include "Resource/GraphicIndexBuffer.h"

namespace YD3D
{
	GraphicIndexBuffer::GraphicIndexBuffer() :
		mBufferLength(0)
	{
	}

	GraphicIndexBuffer::~GraphicIndexBuffer()
	{
	}

	bool GraphicIndexBuffer::Create(ID3D12Device* device, uint64_t length, DXGI_FORMAT format)
	{
		bool b = GraphicResource::Create(
			device,
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(length),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr);

		if (b) 
		{
			mIndexBuffer.BufferLocation = GetGpuAddress();
			mIndexBuffer.SizeInBytes = length;
			mIndexBuffer.Format = format;
			mBufferLength = length;

			return true;
		}
		else 
		{
			return false;
		}

	}

	const D3D12_INDEX_BUFFER_VIEW& GraphicIndexBuffer::IndexView() const
	{
		return mIndexBuffer;
	}

	uint64_t GraphicIndexBuffer::BufferLength()
	{
		return mBufferLength;
	}

	void GraphicIndexBuffer::Release()
	{
		GraphicResource::Release();
		mBufferLength = 0;
		mIndexBuffer = {};
	}

};


