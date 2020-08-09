#include "YD3D_Header.h"
#include "Resource/GraphicVertexBuffer.h"
#include "Resource/GraphicIndexBuffer.h"
#include "Resource/GraphicUploadBuffer.h"

namespace YD3D
{
	class CommonVertexIndexBuffer 
	{
		struct UploadLayout
		{
			UploadLayout() :
				FullScreenVertexOffset(0),
				FullScreenVertexSize(0),
				FullScreenIndexOffset(0),
				FullScreenIndexSize(0)
			{
			}

			uint64_t FullScreenVertexOffset;
			uint64_t FullScreenVertexSize;
			uint64_t FullScreenIndexOffset;
			uint64_t FullScreenIndexSize;
		};


	public:
		static void Initialize(ID3D12Device* device);
		static const GraphicVertexBuffer& GetFullScreenVertexBuffer();
		static const GraphicIndexBuffer& GetFullScreenIndexBuffer();

	private:
		static void InitializeFullScreen(ID3D12Device* device, uint64_t &offset, GraphicUploadBuffer &uploader);
		static void UploadCommonVertexIndexBuffer(ID3D12GraphicsCommandList* commandList, GraphicUploadBuffer& uploader);

		static UploadLayout			_COMMON_VERTEX_INDEX_BUFFER_LAYOUT_;
		static GraphicVertexBuffer	_FULL_SCREEN_RECTANGLE_VERTEX_BUFFER_;
		static GraphicIndexBuffer	_FULL_SCREEN_RECTANGLE_INDEX_BUFFER_;
	};


};

