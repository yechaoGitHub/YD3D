#pragma once

#include "Resource/GraphicResource.h"

namespace YD3D 
{
	class GraphicVertexBuffer : public GraphicResource
	{
	public:
		GraphicVertexBuffer();
		~GraphicVertexBuffer();

		bool Create(ID3D12Device* device, uint32_t stride,  uint64_t count);
		const D3D12_VERTEX_BUFFER_VIEW& VertexView() const;
		uint64_t BufferLength();
		void Release() override;

	private:
		D3D12_VERTEX_BUFFER_VIEW	mVertexBuffer;
		uint64_t					mBufferLength;
	};
}
