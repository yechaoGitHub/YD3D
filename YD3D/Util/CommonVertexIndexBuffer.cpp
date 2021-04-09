#include "CommonVertexIndexBuffer.h"
#include "Graph/GraphicTask.h"
#include "Util/YD3D_Util.h"

using namespace Microsoft::WRL;

namespace YD3D
{
	CommonVertexIndexBuffer::UploadLayout CommonVertexIndexBuffer::_COMMON_VERTEX_INDEX_BUFFER_LAYOUT_;
	GraphicVertexBuffer	CommonVertexIndexBuffer::_FULL_SCREEN_RECTANGLE_VERTEX_BUFFER_;
	GraphicIndexBuffer	CommonVertexIndexBuffer::_FULL_SCREEN_RECTANGLE_INDEX_BUFFER_;

	void CommonVertexIndexBuffer::Initialize(ID3D12Device* device)
	{
		uint64_t offset(0);
		GraphicUploadBuffer uploader;
		uploader.Create(device, 1024);
		InitializeFullScreen(device, offset, uploader);

		uint64_t fenceValue = GraphicTask::PostGraphicTask(
			ECommandQueueType::ECOPY, 
			[&uploader](ID3D12GraphicsCommandList* commandList) 
			{
				UploadCommonVertexIndexBuffer(commandList, uploader);
			}
		);

		GraphicTask::WaitForGraphicTaskCompletion(ECommandQueueType::ECOPY, fenceValue);
	}

	const GraphicVertexBuffer& CommonVertexIndexBuffer::GetFullScreenVertexBuffer()
	{
		return _FULL_SCREEN_RECTANGLE_VERTEX_BUFFER_;
	}

	const GraphicIndexBuffer& CommonVertexIndexBuffer::GetFullScreenIndexBuffer()
	{
		return _FULL_SCREEN_RECTANGLE_INDEX_BUFFER_;
	}

	void CommonVertexIndexBuffer::InitializeFullScreen(ID3D12Device* device, uint64_t &offset, GraphicUploadBuffer& uploader)
	{
		const std::array<Vertex, 4> &vertices = GetFullScreenVertex();
		const std::array<uint32_t, 6> &indices = GetFullScreenIndex();
		uint64_t vertexSize = vertices.size() * sizeof(Vertex);
		uint64_t indexSize = indices.size() * sizeof(uint32_t);
		
		_FULL_SCREEN_RECTANGLE_VERTEX_BUFFER_.Create(device, sizeof(Vertex), 4);
		_FULL_SCREEN_RECTANGLE_INDEX_BUFFER_.Create(device, 6 * sizeof(uint32_t), DXGI_FORMAT_R32_UINT);

		uploader.CopyData(offset, reinterpret_cast<const uint8_t*>(vertices.data()), vertexSize);
		_COMMON_VERTEX_INDEX_BUFFER_LAYOUT_.FullScreenVertexSize = vertexSize;
		_COMMON_VERTEX_INDEX_BUFFER_LAYOUT_.FullScreenVertexOffset = offset;
		offset += vertexSize;
		
		uploader.CopyData(offset, reinterpret_cast<const uint8_t*>(indices.data()), indexSize);
		_COMMON_VERTEX_INDEX_BUFFER_LAYOUT_.FullScreenIndexSize = indexSize;
		_COMMON_VERTEX_INDEX_BUFFER_LAYOUT_.FullScreenIndexOffset = offset;
		indexSize += indexSize;
	}

	void CommonVertexIndexBuffer::UploadCommonVertexIndexBuffer(ID3D12GraphicsCommandList* commandList, GraphicUploadBuffer& uploader)
	{
		commandList->CopyBufferRegion(
			_FULL_SCREEN_RECTANGLE_VERTEX_BUFFER_.Resource(),
			0, 
			uploader.Resource(),
			_COMMON_VERTEX_INDEX_BUFFER_LAYOUT_.FullScreenVertexOffset,
			_COMMON_VERTEX_INDEX_BUFFER_LAYOUT_.FullScreenVertexSize);
		
		commandList->CopyBufferRegion(
			_FULL_SCREEN_RECTANGLE_INDEX_BUFFER_.Resource(),
			0,
			uploader.Resource(),
			_COMMON_VERTEX_INDEX_BUFFER_LAYOUT_.FullScreenIndexOffset,
			_COMMON_VERTEX_INDEX_BUFFER_LAYOUT_.FullScreenIndexSize);
	}
};

