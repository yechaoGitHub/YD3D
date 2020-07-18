#pragma once
#include "GraphicResource.h"

namespace YD3D
{
	class GraphicUploadBuffer : public GraphicResource
	{
	public:
		GraphicUploadBuffer();
		~GraphicUploadBuffer();
		
		bool Create(ID3D12Device* device, uint64_t length, bool autoMapped = true);
		bool CopyData(uint64_t Offset, const BYTE* data, uint64_t lenght);
		uint8_t* GetMappedPointer();

	private:
		uint8_t*		mMappedData;
		uint64_t		mBufferLength;
	};
};

