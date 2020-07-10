#include "GraphicTask.h"

namespace YD3D 
{
	GraphicTask* GraphicTask::_GLOBAL_GRAPHIC_TASK_(nullptr);

	GraphicTask::GraphicTask() :
		mDevice(nullptr)
	{
	}

	GraphicTask::~GraphicTask()
	{
	}

	bool GraphicTask::Create(ID3D12Device* device)
	{
		mDevice = device;
		assert(mCopyCommandQueue.Create(mDevice, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_QUEUE_FLAG_NONE));
		assert(mCopyCommandAllocator.Create(mDevice, D3D12_COMMAND_LIST_TYPE_COPY, 10));
		assert(mRenderCommandQueue.Create(mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_QUEUE_FLAG_NONE));
		assert(mRenderCommandAllocator.Create(mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT, 10));
		assert(mComputeCommandQueue.Create(mDevice, D3D12_COMMAND_LIST_TYPE_COMPUTE, D3D12_COMMAND_QUEUE_FLAG_NONE));
		assert(mComputeCommandAllocator.Create(mDevice, D3D12_COMMAND_LIST_TYPE_COMPUTE, 10));
		assert(mSwapChainCommandQueue.Create(mDevice, D3D12_COMMAND_LIST_TYPE_COMPUTE, D3D12_COMMAND_QUEUE_FLAG_NONE));
		assert(mSwapChainCommandAllocator.Create(mDevice, D3D12_COMMAND_LIST_TYPE_COMPUTE, 10));

		return true;
	}

	bool GraphicTask::PostTask(ECommandQueueType type, GraphicTaskFunction&& task, uint64_t* fenceValue)
	{
		TaskCallbackParam param;
		ID3D12GraphicsCommandList* commandList = GetCommandListAllocator(type).Allocate();
		assert(commandList);
		task(commandList);
		commandList->Close();

		param.Type = SwitchToDX12Type(type);
		param.CommandLists.push_back(commandList);
		param.HasCallback = false;

		return GetCommandQueue(type).PostCommandList(1, &commandList, fenceValue, std::bind(&GraphicTask::GraphicTaskDefaultCallback, this, std::move(param)));
	}

	bool GraphicTask::PostTask(ECommandQueueType type, GraphicTaskFunction&& task, uint64_t* fenceValue, GraphicTaskCallBackFunction&& callback)
	{
		TaskCallbackParam param;
		ID3D12GraphicsCommandList* commandList = GetCommandListAllocator(type).Allocate();
		assert(commandList);
		task(commandList);
		commandList->Close();

		param.Type = SwitchToDX12Type(type);
		param.CommandLists.push_back(commandList);
		param.HasCallback = true;
		param.Callback = std::move(callback);

		return GetCommandQueue(type).PostCommandList(1, &commandList, fenceValue, std::bind(&GraphicTask::GraphicTaskDefaultCallback, this, std::move(param)));
	}

	bool GraphicTask::PostTask(ECommandQueueType type, std::vector<GraphicTaskFunction>&& vecTask, uint64_t* fenceValue)
	{
		TaskCallbackParam param;
		param.Type = SwitchToDX12Type(type);
		param.HasCallback = false;
		CommandListAllocator &allocator = GetCommandListAllocator(type);

		for (auto& task : vecTask) 
		{
			ID3D12GraphicsCommandList* commandList = allocator.Allocate();
			assert(commandList);
			task(commandList);
			param.CommandLists.push_back(commandList);
		}

		return GetCommandQueue(type).PostCommandList(param.CommandLists.size(), param.CommandLists.data(), fenceValue, std::bind(&GraphicTask::GraphicTaskDefaultCallback, this, std::move(param)));;
	}

	bool GraphicTask::PostTask(ECommandQueueType type, std::vector<GraphicTaskFunction>&& vecTask, uint64_t* fenceValue, GraphicTaskCallBackFunction&& callback)
	{
		TaskCallbackParam param;
		param.Type = SwitchToDX12Type(type);
		param.HasCallback = true;
		param.Callback = std::move(callback);
		CommandListAllocator& allocator = GetCommandListAllocator(type);
		for (auto& task : vecTask)
		{
			ID3D12GraphicsCommandList* commandList = allocator.Allocate();
			assert(commandList);
			task(commandList);
			param.CommandLists.push_back(commandList);
		}

		return GetCommandQueue(type).PostCommandList(param.CommandLists.size(), param.CommandLists.data(), fenceValue, std::bind(&GraphicTask::GraphicTaskDefaultCallback, this, std::move(param)));
	}

	CommandQueue& GraphicTask::GetCommandQueue(ECommandQueueType type)
	{
		switch (type)
		{
			case ECommandQueueType::ERENDER:
				return mRenderCommandQueue;

			case ECommandQueueType::ECOPY:
				return mCopyCommandQueue;

			case ECommandQueueType::ECOMPUTE:
				return mComputeCommandQueue;

			case ECommandQueueType::ESWAP_CHAIN:
				return mSwapChainCommandQueue;

			default:
				assert(0);
			break;
		}
		assert(0);
	}

	bool GraphicTask::WaitForCompletion(ECommandQueueType type, uint64_t fenceValue, bool waitCallback)
	{
		return GetCommandQueue(type).WaitForCompletion(fenceValue, waitCallback);;
	}

	void GraphicTask::SetGlobalGraphicTask(GraphicTask &graphicTask)
	{
		_GLOBAL_GRAPHIC_TASK_ = &graphicTask;
	}

	CommandQueue& GraphicTask::GetGraphicTaskCommandQueue(ECommandQueueType type)
	{
		return _GLOBAL_GRAPHIC_TASK_->GetCommandQueue(type);
	}

	bool GraphicTask::PostGraphicTask(ECommandQueueType type, GraphicTaskFunction&& task, uint64_t* fenceValue)
	{
		return _GLOBAL_GRAPHIC_TASK_->PostTask(type, std::move(task), fenceValue);
	}

	bool GraphicTask::PostGraphicTask(ECommandQueueType type, GraphicTaskFunction&& task, uint64_t* fenceValue, GraphicTaskCallBackFunction&& callback)
	{
		return _GLOBAL_GRAPHIC_TASK_->PostTask(type, std::move(task), fenceValue, std::move(callback));
	}

	bool GraphicTask::PostGraphicTask(ECommandQueueType type, std::vector<GraphicTaskFunction>&& task, uint64_t* fenceValue)
	{
		return _GLOBAL_GRAPHIC_TASK_->PostTask(type, std::move(task), fenceValue);
	}

	bool GraphicTask::PostGraphicTask(ECommandQueueType type, std::vector<GraphicTaskFunction>&& task, uint64_t* fenceValue, GraphicTaskCallBackFunction&& callback)
	{
		return _GLOBAL_GRAPHIC_TASK_->PostTask(type, std::move(task), fenceValue, std::move(callback));
	}

	bool GraphicTask::WaitForGraphicTaskCompletion(ECommandQueueType type, uint64_t fenceValue, bool waitCallback)
	{
		return _GLOBAL_GRAPHIC_TASK_->WaitForCompletion(type, fenceValue, waitCallback);
	}

	CommandListAllocator& GraphicTask::GetCommandListAllocator(ECommandQueueType type)
	{
		switch (type)
		{
			case ECommandQueueType::ERENDER:
				return mRenderCommandAllocator;

			case ECommandQueueType::ECOPY:
				return mCopyCommandAllocator;

			case ECommandQueueType::ECOMPUTE:
				return mComputeCommandAllocator;

			case ECommandQueueType::ESWAP_CHAIN:
				return mSwapChainCommandAllocator;

			default:
				assert(0);
			break;
		}

		assert(0);
	}

	D3D12_COMMAND_LIST_TYPE GraphicTask::SwitchToDX12Type(ECommandQueueType type)
	{
		switch (type)
		{
			case ECommandQueueType::ERENDER:
				return D3D12_COMMAND_LIST_TYPE_DIRECT;

			case ECommandQueueType::ECOPY:
				return D3D12_COMMAND_LIST_TYPE_COPY;

			case ECommandQueueType::ECOMPUTE:
				return D3D12_COMMAND_LIST_TYPE_COMPUTE;

			case ECommandQueueType::ESWAP_CHAIN:
				return D3D12_COMMAND_LIST_TYPE_DIRECT;

			default:
				assert(0);
			break;
		}

		assert(0);

		return D3D12_COMMAND_LIST_TYPE_DIRECT;
	}

	void GraphicTask::GraphicTaskDefaultCallback(TaskCallbackParam& param)
	{
		FreeCommandList(param.Type, param.CommandLists);
		if (param.HasCallback) 
		{
			param.Callback();
		}
	}

	void GraphicTask::FreeCommandList(D3D12_COMMAND_LIST_TYPE type, std::vector<ID3D12GraphicsCommandList*>& vecCommandList)
	{
		switch (type)
		{
			case D3D12_COMMAND_LIST_TYPE_COPY:
			{
				for (auto &commandList : vecCommandList)
				{
					mCopyCommandAllocator.Free(commandList);
				}
			}
			break;

			case D3D12_COMMAND_LIST_TYPE_DIRECT:
			{
				for (auto& commandList : vecCommandList)
				{
					mRenderCommandAllocator.Free(commandList);
				}
			}
			break;

			case D3D12_COMMAND_LIST_TYPE_COMPUTE: 
			{
				for (auto& commandList : vecCommandList)
				{
					mComputeCommandAllocator.Free(commandList);
				}
			}
			break;

			default:
				assert(0);
			break;
		}
	}
}
