#pragma once
#include "Resource/GraphicResource.h"

namespace YD3D
{
	class GraphicIndexBuffer : public GraphicResource
	{
	public:
		GraphicIndexBuffer();
		~GraphicIndexBuffer();

		bool Create(ID3D12Device* device, uint64_t length, DXGI_FORMAT format);
		const D3D12_INDEX_BUFFER_VIEW& IndexView();
		uint64_t BufferLength();
		void Release() override;

	private:
		D3D12_INDEX_BUFFER_VIEW		mIndexBuffer;
		uint64_t					mBufferLength;
	};
};

