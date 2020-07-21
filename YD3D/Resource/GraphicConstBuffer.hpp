#pragma once
#include "YD3D_Header.h"
#include "GraphicUploadBuffer.h"

namespace YD3D
{
	template<typename T>
	class GraphicConstBuffer : public GraphicUploadBuffer
	{
	public:
		GraphicConstBuffer() :
			mElementByteSize(0),
			mElementCount(0)
		{

		}

		~GraphicConstBuffer()
		{
			mElementByteSize = 0;
			mElementCount = 0;
		}

		bool Create(ID3D12Device* device, uint32_t count)
		{
			mElementCount = count;
			mElementByteSize = sizeof(T);//CalcConstantBufferByteSize(sizeof(T));
			return GraphicUploadBuffer::Create(device, mElementByteSize * mElementCount);
		}

		bool Update(uint32_t elementIndex, const T& data)
		{
			return CopyData(elementIndex * mElementByteSize, reinterpret_cast<const BYTE*>(&data), mElementByteSize);;
		}

		D3D12_GPU_VIRTUAL_ADDRESS GetElemGpuAddress(int elementIndex) const
		{
			return Resource()->GetGPUVirtualAddress() + elementIndex * mElementByteSize;
		}

		uint32_t ElemCount() 
		{
			return mElementCount;
		}

	private:
		uint32_t	mElementByteSize;
		uint32_t	mElementCount;
	};
};
