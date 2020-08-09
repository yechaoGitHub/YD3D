#include "UploadHelper.h"
#include "Graph/GraphicTask.h"
#include "Util/YD3D_Util.h"

namespace YD3D
{
	GraphicUploadBuffer							UploadHelper::_UPLOADER_;
	std::condition_variable						UploadHelper::_UPLOAD_CON_VAR_;
	std::mutex									UploadHelper::_UPLOAD_LOCK_;
	wait_free_queue<UploadHelper::UploadLayout>	UploadHelper::_UPLOAD_LAYOUT_;
	ythread<void>								UploadHelper::_UPLOAD_THREAD_;
	std::atomic<uint64_t>						UploadHelper::_UPLOAD_FENCE_(0);
	
	void UploadHelper::Initialize(ID3D12Device* device)
	{
		_UPLOADER_.Create(device, 1024);
		_UPLOAD_THREAD_.run_loop(&UploadHelper::UploadFunctionLoop);
	}

	uint64_t UploadHelper::UploadBuffer(GraphicResource* destRes, uint64_t destOffset, const uint8_t* data, uint64_t dataLength, bool wait)
	{
		UploadLayout layout;
		layout.Data = data;
		layout.DataLength = dataLength;
		layout.IsBuffer = true;
		layout.DestResource = destRes->Resource();
		layout.BufferStartOffset = destOffset;
		
		uint64_t ret = _UPLOAD_LAYOUT_.enqueue(layout) + 1;
		_UPLOAD_CON_VAR_.notify_one();

		if (wait) 
		{
			WaitForUpload(ret);
		}

		return ret;
	}

	uint64_t UploadHelper::UploadTexture(GraphicResource* destRes, uint32_t startSubresourceIndex, const uint8_t* data, uint64_t dataLength, bool wait)
	{
		UploadLayout layout;
		layout.Data = data;
		layout.DataLength = dataLength;
		layout.IsBuffer = false;
		layout.DestResource = destRes->Resource();
		layout.StartSubresourceIndex = startSubresourceIndex;
		
		uint64_t ret = _UPLOAD_LAYOUT_.enqueue(layout) + 1;
		_UPLOAD_CON_VAR_.notify_one();

		if (wait)
		{
			WaitForUpload(ret);
		}

		return ret;
	}

	void UploadHelper::WaitForUpload(uint64_t fence)
	{
		while (fence > _UPLOAD_FENCE_) 
		{
			std::this_thread::yield();
		}
	}

	void UploadHelper::UploadFunctionLoop()
	{
		UploadLayout layout[10] = {};
		uint64_t fenceValue(0);
		uint64_t dequeueCount = _UPLOAD_LAYOUT_.dequeue_range(layout, sizeof(layout), &fenceValue);
		if (dequeueCount) 
		{
			uint64_t dataLenth = CountDataLength(layout, dequeueCount);
			if (dataLenth > _UPLOADER_.GetResByteSize()) 
			{
				ID3D12Device *device = _UPLOADER_.GetDevice();
				_UPLOADER_.Release();
				_UPLOADER_.Create(device, dataLenth * 1.5);
			}

			uint64_t offset(0);
			for (uint32_t i = 0; i < dequeueCount; i++)
			{
				const UploadLayout& v = layout[i];
				_UPLOADER_.CopyData(offset, v.Data, v.DataLength);
				offset += v.DataLength;
			}
			
			uint64_t uploadValue = GraphicTask::PostGraphicTask(ECommandQueueType::ECOPY, &UploadHelper::UploadTask, layout, dequeueCount, std::placeholders::_1);
			GraphicTask::WaitForGraphicTaskCompletion(ECommandQueueType::ECOPY, uploadValue);

			_UPLOAD_FENCE_ += fenceValue + dequeueCount;
		}
		else 
		{
			std::unique_lock<std::mutex> ulock(_UPLOAD_LOCK_);
			_UPLOAD_CON_VAR_.wait(ulock, [] { return _UPLOAD_LAYOUT_.size(); });
		}
	}

	void UploadHelper::UploadTask(const UploadLayout* layout, uint64_t count, ID3D12GraphicsCommandList* commandList)
	{
		uint64_t offset(0);
		for (uint32_t i = 0; i < count; i++)
		{
			const UploadLayout& v = layout[i];
			commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(v.DestResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
			if (v.IsBuffer)
			{
				commandList->CopyBufferRegion(v.DestResource, v.BufferStartOffset, _UPLOADER_.Resource(), offset, v.DataLength);
			}
			else
			{
				D3D12_PLACED_SUBRESOURCE_FOOTPRINT&& footPrint = GetResourceCopyableFootPrint(v.DestResource, v.StartSubresourceIndex);
				CD3DX12_TEXTURE_COPY_LOCATION dest(v.DestResource, v.StartSubresourceIndex);
				CD3DX12_TEXTURE_COPY_LOCATION src(_UPLOADER_.Resource(), footPrint);
				commandList->CopyTextureRegion(&dest, 0, 0, 0, &src, nullptr);
			}
			commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(v.DestResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON));
		}
	}

	uint64_t UploadHelper::CountDataLength(UploadLayout* layout, uint64_t count)
	{
		uint64_t dataLength(0);
		std::for_each(layout, layout + count, 
			[&dataLength](const UploadLayout& v)
			{
				dataLength += v.DataLength;
			});
		
		return dataLength;
	}

};

