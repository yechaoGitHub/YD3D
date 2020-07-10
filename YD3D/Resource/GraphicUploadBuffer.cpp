#include "GraphicUploadBuffer.h"


namespace YD3D
{
	GraphicUploadBuffer::GraphicUploadBuffer():
		mMappedData(nullptr),
		mBufferLength(0)
	{
	}

	GraphicUploadBuffer::~GraphicUploadBuffer()
	{
		if (mMappedData) 
		{
			Unmap(0, nullptr);
		}
	}

	bool GraphicUploadBuffer::Create(ID3D12Device* device, uint64_t length, bool autoMapped)
	{
		bool ret = GraphicResource::Create(
			device,
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(length),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr);

		if (ret & autoMapped)
		{
			mBufferLength = length;
			mMappedData = reinterpret_cast<BYTE*>(Map(0, nullptr));
		}

		return  ret;
	}

	bool GraphicUploadBuffer::CopyData(uint64_t Offset, const BYTE* data, uint64_t lenght)
	{
		if (mMappedData)
		{
			memcpy(&mMappedData[Offset], data, lenght);
			return true;
		}

		return false;
	}

	BYTE* GraphicUploadBuffer::GetMappedPointer()
	{
		return mMappedData;
	}

};
