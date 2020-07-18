#include "CommandQueue.h"

namespace YD3D
{
	CommandQueue::CommandQueue():
		mDevice(nullptr),
		mType(D3D12_COMMAND_LIST_TYPE_DIRECT),
		mFenceValue(0),
		mCommandCount(0),
		mCallbackFence(0),
		mIdleQueueCount(0),
		mIdleCallbackCount(0),
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
		mType = commandQueueType;

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
		mIdleQueueCount = 0;
		mQueueConVar.notify_one();
		if (fenceValue)
		{
			*fenceValue = nxtFenceValue + count;
		}

		return true;
	}

	bool CommandQueue::PostCommandList(uint32_t count, ID3D12GraphicsCommandList** commandList, uint64_t* fenceValue, CommandQueueCallbackFunction&& completionCallback)
	{
		uint64_t nxtFenceValue = mCmdQueue.enqueue_range(commandList, count);
		mIdleQueueCount = 0;
		mQueueConVar.notify_one();
		if (fenceValue)
		{
			*fenceValue = nxtFenceValue + count;
		}

		CompletionCallback callback;
		callback.fenceValue = nxtFenceValue + count;
		callback.completionCallback = std::move(completionCallback);

		mCompletionQueue.enqueue(std::move(callback));
		mIdleCallbackCount = 0;
		mCallbackConVar.notify_one();

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
			mIdleQueueCount = 0;
		}
		else 
		{
			mIdleQueueCount++;
			std::this_thread::yield();
			if (mIdleQueueCount > 20)
			{
				std::unique_lock<std::mutex> uLock(mQueueLock);
				mQueueConVar.wait(uLock, [this] {return mCmdQueue.size(); });
			}
		}
	}

	void CommandQueue::CallbackWorkFunction()
	{
		CompletionCallback callback;

		if (mCompletionQueue.dequeue(callback)) 
		{
			WaitForCompletion(callback.fenceValue, false);
			callback.completionCallback(mType, callback.fenceValue);
			mCallbackFence = callback.fenceValue;
			mIdleCallbackCount = 0;
		}
		else 
		{
			mIdleCallbackCount++;
			std::this_thread::yield();
			if (mIdleCallbackCount > 20)
			{
				std::unique_lock<std::mutex> uLock(mCallbackLock);
				mCallbackConVar.wait(uLock, [this] { return mCompletionQueue.size(); });
			}
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

