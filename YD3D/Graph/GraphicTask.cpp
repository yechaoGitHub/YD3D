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
		//assert(mCopyCommandQueue.Create(mDevice, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_QUEUE_FLAG_NONE));
		//assert(mCopyCommandAllocator.Create(mDevice, D3D12_COMMAND_LIST_TYPE_COPY, 10));
		//assert(mRenderCommandQueue.Create(mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_QUEUE_FLAG_NONE));
		//assert(mRenderCommandAllocator.Create(mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT, 10));
		//assert(mComputeCommandQueue.Create(mDevice, D3D12_COMMAND_LIST_TYPE_COMPUTE, D3D12_COMMAND_QUEUE_FLAG_NONE));
		//assert(mComputeCommandAllocator.Create(mDevice, D3D12_COMMAND_LIST_TYPE_COMPUTE, 10));
		assert(mSwapChainCommandQueue.Create(mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_QUEUE_FLAG_NONE));
		assert(mSwapChainCommandAllocator.Create(mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT, 10));

		return true;
	}

	bool GraphicTask::PostTask(ECommandQueueType type, GraphicTaskFunction&& task, uint64_t* fenceValue)
	{
		TaskCallbackParam param;
		ID3D12GraphicsCommandList* commandList = AllocateCommandList(type);
		assert(commandList);
		task(commandList);
		commandList->Close();

		param.Type = type;
		param.CommandList = commandList;
		param.HasCallback = false;

		return GetCommandQueue(type).PostCommandList(1, &commandList, fenceValue, std::bind(&GraphicTask::GraphicTaskDefaultCallback, this, std::move(param), std::placeholders::_1, std::placeholders::_2));
	}

	bool GraphicTask::PostTask(ECommandQueueType type, GraphicTaskFunction&& task, uint64_t* fenceValue, GraphicTaskCallbackFunction&& callback)
	{
		TaskCallbackParam param;
		ID3D12GraphicsCommandList* commandList = AllocateCommandList(type);
		assert(commandList);
		task(commandList);
		commandList->Close();

		param.Type = type;
		param.CommandList = commandList;
		param.HasCallback = true;
		param.Callback = std::move(callback);

		return GetCommandQueue(type).PostCommandList(1, &commandList, fenceValue, std::bind(&GraphicTask::GraphicTaskDefaultCallback, this, std::move(param), std::placeholders::_1, std::placeholders::_2));
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

	GraphicTask* GraphicTask::GlobalGraphicTask()
	{
		return _GLOBAL_GRAPHIC_TASK_;
	}

	CommandQueue& GraphicTask::GetGraphicTaskCommandQueue(ECommandQueueType type)
	{
		return _GLOBAL_GRAPHIC_TASK_->GetCommandQueue(type);
	}

	bool GraphicTask::PostGraphicTask(ECommandQueueType type, GraphicTaskFunction&& task, uint64_t* fenceValue)
	{
		return _GLOBAL_GRAPHIC_TASK_->PostTask(type, std::move(task), fenceValue);
	}

	bool GraphicTask::PostGraphicTask(ECommandQueueType type, GraphicTaskFunction&& task, uint64_t* fenceValue, GraphicTaskCallbackFunction&& callback)
	{
		return _GLOBAL_GRAPHIC_TASK_->PostTask(type, std::move(task), fenceValue, std::move(callback));
	}

	bool GraphicTask::WaitForGraphicTaskCompletion(ECommandQueueType type, uint64_t fenceValue, bool waitCallback)
	{
		return _GLOBAL_GRAPHIC_TASK_->WaitForCompletion(type, fenceValue, waitCallback);
	}

	void GraphicTask::GraphicTaskDefaultCallback(TaskCallbackParam& param, D3D12_COMMAND_LIST_TYPE type, uint64_t fence)
	{
		FreeCommandList(param.Type, param.CommandList);
		if (param.HasCallback) 
		{
			param.Callback(type, fence);
		}
	}

	ID3D12GraphicsCommandList* GraphicTask::AllocateCommandList(ECommandQueueType type)
	{
		switch (type)
		{
		case YD3D::ERENDER:
			return mRenderCommandAllocator.Allocate();

		case YD3D::ECOPY:
			return mCopyCommandAllocator.Allocate();

		case YD3D::ECOMPUTE:
			return mComputeCommandAllocator.Allocate();

		case YD3D::ESWAP_CHAIN:
			return mSwapChainCommandAllocator.Allocate();

		default:
			assert(0);
			break;
		}

		return nullptr;
	}

	bool GraphicTask::ResetCommandList(ECommandQueueType type, ID3D12GraphicsCommandList* commandList)
	{
		switch (type)
		{
			case ECommandQueueType::ECOPY:
			{
				return mCopyCommandAllocator.Reset(commandList);
			}
			break;

			case ECommandQueueType::ERENDER:
			{
				return mRenderCommandAllocator.Reset(commandList);
			}
			break;

			case ECommandQueueType::ECOMPUTE:
			{
				return mComputeCommandAllocator.Reset(commandList);
			}
			break;

			case ECommandQueueType::ESWAP_CHAIN:
			{
				return mSwapChainCommandAllocator.Reset(commandList);
			}
			break;

			default:
				assert(0);
			break;
		}

		return false;
	}

	bool GraphicTask::FreeCommandList(ECommandQueueType type, ID3D12GraphicsCommandList* commandList)
	{
		switch (type)
		{
			case ECommandQueueType::ECOPY:
			{
				return mCopyCommandAllocator.Free(commandList);
			}
			break;

			case ECommandQueueType::ERENDER:
			{
				return mRenderCommandAllocator.Free(commandList);
			}
			break;

			case ECommandQueueType::ECOMPUTE:
			{
				return mComputeCommandAllocator.Free(commandList);
			}
			break;

			case ECommandQueueType::ESWAP_CHAIN:
			{
				return mSwapChainCommandAllocator.Free(commandList);
			}
			break;

			default:
				assert(0);
			break;
		}

		return false;
	}
}
