#pragma once
#include "YD3D_Header.h"
#include "Resource/GraphicUploadBuffer.h"
#include "Util/wait_free_queue.hpp"

namespace YD3D
{
	class UploadHelper 
	{
		struct UploadLayout
		{
			UploadLayout() :
				Data(nullptr),
				DataLength(0),
				IsBuffer(false),
				DestResource(nullptr)
			{

			}

			const uint8_t*	Data;
			uint64_t		DataLength;
			bool			IsBuffer;
			ID3D12Resource* DestResource;

			union 
			{
				uint64_t BufferStartOffset;
				uint64_t StartSubresourceIndex;
			};
		};

	public:
		static void Initialize(ID3D12Device *device);

		static uint64_t UploadBuffer(GraphicResource *destRes, uint64_t destOffset, const uint8_t* data, uint64_t dataLength, bool wait = true);
		static uint64_t UploadTexture(GraphicResource *destRes, uint32_t startSubresourceIndex, const uint8_t* data, uint64_t dataLength, bool wait = true);
		static void WaitForUpload(uint64_t fence);

	private:
		static GraphicUploadBuffer				_UPLOADER_;
		static std::condition_variable			_UPLOAD_CON_VAR_;
		static std::mutex						_UPLOAD_LOCK_;
		static wait_free_queue<UploadLayout>	_UPLOAD_LAYOUT_;
		static ythread<void>					_UPLOAD_THREAD_;
		static std::atomic<uint64_t>			_UPLOAD_FENCE_;

		static void UploadFunctionLoop();
		static void UploadTask(const UploadLayout *layout, uint64_t count, ID3D12GraphicsCommandList *commandList);
		static uint64_t CountDataLength(UploadLayout* layout, uint64_t count);

	};
};
