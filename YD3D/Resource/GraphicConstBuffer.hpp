#pragma once
#include "YD3D_Header.h"
#include "GraphicUploadBuffer.h"

namespace YD3D
{
	template<typename T, uint32_t ElemCount>
	class GraphicConstBuffer : public GraphicUploadBuffer
	{
	public:
		GraphicConstBuffer() :
			mElementByteSize(0),
			mElementCount(ElemCount)
		{

		}

		~GraphicConstBuffer()
		{

		}

		bool Create(ID3D12Device* device)
		{
			mElementByteSize = CalcConstantBufferByteSize(sizeof(T));
			return GraphicUploadBuffer::Create(device, mElementByteSize * mElementCount);
		}

		bool Update(int elementIndex, const T& data)
		{
			return CopyData(elementIndex * mElementByteSize, &data, mElementByteSize);;
		}

		D3D12_GPU_VIRTUAL_ADDRESS GetElemGpuAddress(int elementIndex) const
		{
			return Resource()->GetGPUVirtualAddress() + elementIndex * mElementByteSize;
		}

	private:
		uint32_t	mElementByteSize;
		uint32_t	mElementCount;
	};
};
