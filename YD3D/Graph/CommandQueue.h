#include "YD3D_Header.h"
#include "Util/wait_free_queue.hpp"

namespace YD3D
{
	struct CompletionCallback
	{
		uint64_t						fenceValue;
		CommandQueueCallbackFunction	completionCallback;
	};

	class CommandQueue 
	{
	public:
		CommandQueue();
		~CommandQueue();

		bool Create(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE commandQueueType, D3D12_COMMAND_QUEUE_FLAGS commandQueueFlag);

		ID3D12CommandQueue* Queue();
		uint64_t PostCommandList(uint32_t count, ID3D12GraphicsCommandList** commandList);
		uint64_t PostCommandList(uint32_t count, ID3D12GraphicsCommandList** commandList, CommandQueueCallbackFunction&& completionCallback);
		bool WaitForCompletion(uint64_t fenceValue, bool waitForCallback = true);

	private:
		void CommandQueueThreadFunction();
		void CallbackWorkFunction();
		bool ExcuteQueue(ID3D12CommandList** commandList, uint32_t count);

		ID3D12Device*									mDevice;
		D3D12_COMMAND_LIST_TYPE							mType;
		std::atomic<uint64_t>							mFenceValue;
		std::atomic<uint64_t>							mCommandCount;
		std::atomic<uint64_t>							mCallbackFence;

		std::atomic<uint64_t>							mIdleQueueCount;
		std::atomic<uint64_t>							mIdleCallbackCount;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue>		mCommandQueue;
		Microsoft::WRL::ComPtr<ID3D12Fence>				mFence;
		wait_free_queue<ID3D12GraphicsCommandList*>		mCmdQueue;
		wait_free_queue<CompletionCallback>				mCompletionQueue;
		ythread<void>									mCommandListThread;
		ythread<void>									mCallbackThread;
		std::mutex										mQueueLock;
		std::condition_variable							mQueueConVar;
		std::mutex										mCallbackLock;
		std::condition_variable							mCallbackConVar;
	};

};
