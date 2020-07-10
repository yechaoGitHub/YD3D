#include "CommandQueue.h"

namespace YD3D
{
	CommandQueue::CommandQueue():
		mDevice(nullptr),
		mFenceValue(0),
		mCommandCount(0),
		mCallbackFence(0),
		mCmdQueue(100),
		mCompletionQueue(100)
	{
	}

	CommandQueue::~CommandQueue()
	{
	}

	bool CommandQueue::Create(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE commandQueueType, D3D12_COMMAND_QUEUE_FLAGS commandQueueFlag)
	{
		mDevice = device;

		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = commandQueueType;
		desc.Flags = commandQueueFlag;

		ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&mCommandQueue)));
		ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));

		assert(mCallbackThread.run_loop(&CommandQueue::CallbackWorkFunction, this));
		assert(mCommandListThread.run_loop(&CommandQueue::CommandQueueThreadFunction, this));

		return true;
	}

	ID3D12CommandQueue* CommandQueue::Queue()
	{
		return mCommandQueue.Get();
	}

	bool CommandQueue::PostCommandList(uint32_t count, ID3D12GraphicsCommandList** commandList, uint64_t* fenceValue)
	{
		uint64_t nxtFenceValue = mCmdQueue.enqueue_range(commandList, count);
		if (fenceValue)
		{
			*fenceValue = nxtFenceValue + count;
		}

		return true;
	}

	bool CommandQueue::PostCommandList(uint32_t count, ID3D12GraphicsCommandList** commandList, uint64_t* fenceValue, std::function<void(void)>&& completionCallback)
	{
		uint64_t nxtFenceValue = mCmdQueue.enqueue_range(commandList, count);
		if (fenceValue)
		{
			*fenceValue = nxtFenceValue + count;
		}

		CompletionCallback callback;
		callback.fenceValue = nxtFenceValue + count;
		callback.completionCallback = std::move(completionCallback);

		mCompletionQueue.enqueue(std::move(callback));

		return true;
	}

	bool CommandQueue::WaitForCompletion(uint64_t fenceValue, bool waitForCallback)
	{
		bool uncompleted(false);
		bool callbackUncompleted(false);
		
		do
		{
			uncompleted = mFenceValue < fenceValue;
			callbackUncompleted = mFenceValue < mCallbackFence;
		} 
		while (uncompleted | (waitForCallback & callbackUncompleted));

		return true;
	}

	void CommandQueue::CommandQueueThreadFunction()
	{
		ID3D12GraphicsCommandList* commandListBuffer[10] = {};
		uint64_t count = mCmdQueue.dequeue_range(commandListBuffer, 10 * sizeof(ID3D12CommandList));
		if (count) 
		{
			ExcuteQueue(reinterpret_cast<ID3D12CommandList**>(commandListBuffer), count);
		}
		else 
		{
			std::this_thread::yield();
		}
	}

	void CommandQueue::CallbackWorkFunction()
	{
		CompletionCallback callback;

		if (mCompletionQueue.dequeue(callback)) 
		{
			WaitForCompletion(callback.fenceValue, false);
			callback.completionCallback();
			mCallbackFence = callback.fenceValue;
		}
		else 
		{
			std::this_thread::yield();
		}
	}

	bool CommandQueue::ExcuteQueue(ID3D12CommandList** commandList, uint32_t count)
	{
		mCommandQueue->ExecuteCommandLists(count, commandList);

		uint64_t fenceValue = mFenceValue + count;
		ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), fenceValue));

		if (mFence->GetCompletedValue() < fenceValue)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
			ThrowIfFailed(mFence->SetEventOnCompletion(fenceValue, eventHandle));
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);

		}

		mFenceValue += count;

		return true;
	}

	

};

